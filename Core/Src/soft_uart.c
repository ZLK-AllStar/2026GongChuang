#include "soft_uart.h"
#include "main.h"
#include "app_config.h"
#include "app_mode.h"
#include "app_runtime.h"
#include "shell_port.h"
#include <string.h>

#if APP_SHELL_SOFT_UART_SAMPLES != 8U || APP_SHELL_SOFT_UART_TX_SIZE < 2U || \
    APP_SHELL_SOFT_UART_TX_SIZE > 65535U || APP_SHELL_SOFT_UART_DIVISOR > 65536U || \
    APP_SHELL_SOFT_UART_DIVISOR < 1U
#error Unsupported software UART configuration
#endif

#define TICK_CYCLES (APP_SHELL_SOFT_UART_DIVISOR * \
  (APP_SHELL_SOFT_UART_CPU_HZ / APP_SHELL_SOFT_UART_TIMER_HZ))
#define JITTER_CYCLES (APP_SHELL_SOFT_UART_CPU_HZ / 1000000U * APP_SHELL_SOFT_UART_JITTER_US)
#define LATE_TIMER_TICKS (APP_SHELL_SOFT_UART_TIMER_HZ / 1000000U * APP_SHELL_SOFT_UART_JITTER_US)
#define FRAME_TICKS (10U * APP_SHELL_SOFT_UART_SAMPLES)

typedef enum { RX_WAIT_IDLE, RX_IDLE, RX_START, RX_DATA, RX_STOP } RxState;
static volatile bool running;
static volatile RxState rx_state;
static uint8_t rx_wait, rx_bits, rx_data, idle_high;
static uint32_t rx_generation, last_cycle;
static volatile bool tx_active;
static uint8_t tx_wait, tx_bit, recovery_ticks;
static uint16_t tx_frame;
static volatile uint8_t tx_ring[APP_SHELL_SOFT_UART_TX_SIZE];
static volatile uint16_t tx_head, tx_tail;
static volatile SoftUart_Diagnostics diagnostics;

static bool ReadRx(void)
{
  return (APP_SHELL_SOFT_UART_RX_PORT->IDR & APP_SHELL_SOFT_UART_RX_PIN) != 0U;
}

static void WriteTx(bool high)
{
  APP_SHELL_SOFT_UART_TX_PORT->BSRR = high ? APP_SHELL_SOFT_UART_TX_PIN :
    (uint32_t)APP_SHELL_SOFT_UART_TX_PIN << 16U;
}

bool SoftUart_IsRunning(void) { return running; }

void SoftUart_Stop(void)
{
  uint32_t irq_state = __get_PRIMASK();
  __disable_irq();
  TIM13->DIER &= ~TIM_DIER_UIE;
  TIM13->CR1 &= ~TIM_CR1_CEN;
  TIM13->SR = ~TIM_SR_UIF;
  if (running) {
    if (tx_active || tx_head != tx_tail) { diagnostics.tx_faults++; }
    WriteTx(true);
    ShellPort_RxErrorFromISR();
  }
  running = false;
  tx_active = false;
  tx_tail = tx_head;
  __set_PRIMASK(irq_state);
}

bool SoftUart_Init(void)
{
  GPIO_InitTypeDef gpio = {0};
  /* Fail closed if the clock assumptions change. Do not alter HOST GPIO/timer. */
  if (AppMode_Get() != APP_MODE_DEBUG) { return false; }
  if (running) { return true; }
  if (HAL_RCC_GetHCLKFreq() != APP_SHELL_SOFT_UART_CPU_HZ ||
      HAL_RCC_GetPCLK1Freq() * 2U != APP_SHELL_SOFT_UART_TIMER_HZ ||
      (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) != 0U) { return false; }

  APP_SHELL_SOFT_UART_GPIO_CLK_ENABLE();
  WriteTx(true); /* Set output latch BEFORE enabling push-pull output. */
  gpio.Pin = APP_SHELL_SOFT_UART_TX_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(APP_SHELL_SOFT_UART_TX_PORT, &gpio);
  gpio.Pin = APP_SHELL_SOFT_UART_RX_PIN;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(APP_SHELL_SOFT_UART_RX_PORT, &gpio);

  __HAL_RCC_TIM13_CLK_ENABLE();
  TIM13->DIER = 0U;
  TIM13->CR1 = TIM_CR1_URS;
  TIM13->PSC = 0U;
  TIM13->ARR = APP_SHELL_SOFT_UART_DIVISOR - 1U;
  TIM13->CNT = 0U;
  TIM13->EGR = TIM_EGR_UG;
  TIM13->SR = ~TIM_SR_UIF;
  tx_head = tx_tail = 0U;
  tx_active = false;
  recovery_ticks = 0U;
  rx_state = RX_WAIT_IDLE;
  idle_high = 0U;
  memset((void *)&diagnostics, 0, sizeof(diagnostics));
  /* Never reset CYCCNT: other diagnostics may already use it. */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0U, 0U);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  /* Do not clear shared NVIC pending: a TIM8 event may be waiting. */
  last_cycle = DWT->CYCCNT;
  running = true;
  TIM13->DIER = TIM_DIER_UIE;
  TIM13->CR1 |= TIM_CR1_CEN;
  return true;
}

bool SoftUart_IsRxActive(void)
{
  return running && ((rx_state != RX_IDLE && rx_state != RX_WAIT_IDLE) || !ReadRx());
}

static void TickRx(bool high)
{
  switch (rx_state) {
    case RX_WAIT_IDLE:
      if (!high) { idle_high = 0U; }
      else if (++idle_high >= APP_SHELL_SOFT_UART_SAMPLES) { rx_state = RX_IDLE; }
      break;
    case RX_IDLE:
      if (!high) {
        rx_generation = ShellPort_RxGenerationFromISR();
        rx_wait = APP_SHELL_SOFT_UART_SAMPLES / 2U;
        rx_state = RX_START;
      }
      break;
    case RX_START:
      if (--rx_wait == 0U) {
        if (high) { diagnostics.false_starts++; rx_state = RX_IDLE; }
        else {
          rx_data = rx_bits = 0U;
          rx_wait = APP_SHELL_SOFT_UART_SAMPLES;
          rx_state = RX_DATA;
        }
      }
      break;
    case RX_DATA:
      if (--rx_wait == 0U) {
        if (high) { rx_data |= (uint8_t)(1U << rx_bits); }
        rx_wait = APP_SHELL_SOFT_UART_SAMPLES;
        if (++rx_bits == 8U) { rx_state = RX_STOP; }
      }
      break;
    case RX_STOP:
      if (--rx_wait == 0U) {
        if (high) {
          diagnostics.rx_bytes++;
          rx_state = RX_IDLE; /* No extra idle bit between back-to-back frames. */
          ShellPort_RxByteFromISR(rx_data, rx_generation);
        } else {
          diagnostics.frame_errors++;
          rx_state = RX_WAIT_IDLE;
          idle_high = 0U;
          ShellPort_RxErrorFromISR();
        }
      }
      break;
  }
}

static void TickTx(void)
{
  if (recovery_ticks != 0U) { recovery_ticks--; return; }
  if (tx_active) {
    if (--tx_wait != 0U) { return; }
    if (++tx_bit < 10U) {
      WriteTx(((tx_frame >> tx_bit) & 1U) != 0U);
      tx_wait = APP_SHELL_SOFT_UART_SAMPLES;
      return;
    }
    diagnostics.tx_bytes++;
    tx_active = false;
  }
  if (tx_head != tx_tail) {
    tx_frame = (uint16_t)((uint16_t)tx_ring[tx_tail] << 1U) | 0x200U;
    tx_tail = (uint16_t)((tx_tail + 1U) % APP_SHELL_SOFT_UART_TX_SIZE);
    tx_bit = 0U;
    tx_wait = APP_SHELL_SOFT_UART_SAMPLES;
    tx_active = true;
    WriteTx(false);
  }
}

void SoftUart_TickFromISR(void)
{
  uint32_t now, gap, late;
  if (!running) { return; }
  now = DWT->CYCCNT;
  gap = (uint32_t)(now - last_cycle); /* Unsigned subtraction handles wrap. */
  last_cycle = now;
  late = TIM13->CNT;
  if (gap > diagnostics.max_gap_cycles) { diagnostics.max_gap_cycles = gap; }
  if (late > diagnostics.max_late_timer_ticks) { diagnostics.max_late_timer_ticks = late; }
  if (gap > TICK_CYCLES + JITTER_CYCLES || late > LATE_TIMER_TICKS) {
    diagnostics.timing_errors++;
    rx_state = RX_WAIT_IDLE;
    idle_high = 0U;
    ShellPort_RxErrorFromISR();
    if (tx_active || tx_head != tx_tail) { diagnostics.tx_faults++; }
    tx_active = false;
    tx_tail = tx_head;
    recovery_ticks = FRAME_TICKS;
    WriteTx(true);
    return; /* Lost physical samples cannot be reconstructed by catch-up ticks. */
  }
  TickRx(ReadRx());
  TickTx();
}

void SoftUart_HandleIRQ(void)
{
  if (running && (TIM13->SR & TIM_SR_UIF) != 0U &&
      (TIM13->DIER & TIM_DIER_UIE) != 0U) {
    TIM13->SR = ~TIM_SR_UIF; /* rc_w0: never touch TIM8 or other TIM13 flags. */
    SoftUart_TickFromISR();
  }
}

uint16_t SoftUart_Write(const uint8_t *data, uint16_t length)
{
  uint16_t accepted = 0U, queued;
  uint32_t start, timeout, faults;
  uint64_t wire_ms;
  if (!running || data == 0 || __get_IPSR() != 0U || __get_PRIMASK() != 0U) {
    return 0U;
  }
  faults = diagnostics.tx_faults;
  queued = (uint16_t)((tx_head + APP_SHELL_SOFT_UART_TX_SIZE - tx_tail) % APP_SHELL_SOFT_UART_TX_SIZE);
  /* Include queued frames, an in-flight frame and a recovery idle frame.
   * Use the ACTUAL divider-derived wire time, rounded upwards. */
  wire_ms = (uint64_t)(queued + (uint32_t)length + 2U) * FRAME_TICKS *
    APP_SHELL_SOFT_UART_DIVISOR * 1000U;
  timeout = (uint32_t)((wire_ms + APP_SHELL_SOFT_UART_TIMER_HZ - 1U) /
    APP_SHELL_SOFT_UART_TIMER_HZ) + APP_SHELL_SOFT_UART_MARGIN_MS;
  start = HAL_GetTick();
  while (accepted < length) {
    uint32_t irq_state = __get_PRIMASK();
    uint16_t next;
    bool published = false;
    /* One byte only: atomic with ISR queue cancellation on timing faults. */
    __disable_irq();
    if (!running || faults != diagnostics.tx_faults) {
      __set_PRIMASK(irq_state);
      break;
    }
    next = (uint16_t)((tx_head + 1U) % APP_SHELL_SOFT_UART_TX_SIZE);
    if (next != tx_tail) {
      tx_ring[tx_head] = data[accepted];
      tx_head = next;
      published = true;
    }
    __set_PRIMASK(irq_state);
    if (published) { accepted++; continue; }
    if ((uint32_t)(HAL_GetTick() - start) >= timeout) {
      diagnostics.tx_timeouts++;
      break;
    }
    AppRuntime_Service(); /* Never ShellPort_Poll or mechanical dispatch. */
  }
  return accepted;
}

void SoftUart_GetDiagnostics(SoftUart_Diagnostics *result)
{
  /* Each aligned counter is atomic; this is intentionally a non-coherent
   * snapshot to avoid masking interrupts for a structure-sized copy. */
  if (result == 0) { return; }
  result->rx_bytes = diagnostics.rx_bytes;
  result->tx_bytes = diagnostics.tx_bytes;
  result->frame_errors = diagnostics.frame_errors;
  result->false_starts = diagnostics.false_starts;
  result->timing_errors = diagnostics.timing_errors;
  result->tx_faults = diagnostics.tx_faults;
  result->tx_timeouts = diagnostics.tx_timeouts;
  result->max_gap_cycles = diagnostics.max_gap_cycles;
  result->max_late_timer_ticks = diagnostics.max_late_timer_ticks;
}
