#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Core/Src/soft_uart.c"
#include "../../Core/Src/stm32f4xx_it.c"

GPIO_TypeDef test_gpio;
TIM_TypeDef test_tim8, test_tim13;
TestDWT test_dwt;
TestCoreDebug test_debug;
uint32_t test_primask, test_ipsr, test_hclk = 168000000U, test_pclk = 42000000U;
TIM_HandleTypeDef htim8 = {TIM8}, htim13 = {TIM13};
UART_HandleTypeDef huart1, huart2, huart5;
DMA_HandleTypeDef hdma_usart1_rx, hdma_usart1_tx;
__IO uint8_t rxDmaBuffer[CMD_LEN];
static App_Mode_t selected_mode = APP_MODE_DEBUG;
static uint8_t wave[400000];
static unsigned wave_n, tim8_calls, tim13_calls, gpio_calls;
#ifndef SOFT_UART_SHELL_WIRE
static uint8_t received[8192], expected[4096];
static uint32_t generations[8192], generation;
static unsigned received_n, errors_n;
#endif
static bool auto_drain;
static uint32_t milliseconds, subcycles;
static unsigned service_calls;

App_Mode_t AppMode_Get(void) { return selected_mode; }
#ifndef SOFT_UART_SHELL_WIRE
uint32_t ShellPort_RxGenerationFromISR(void) { return generation; }
void ShellPort_RxByteFromISR(uint8_t byte, uint32_t token)
{
  assert(received_n < sizeof(received));
  generations[received_n] = token;
  received[received_n++] = byte;
}
void ShellPort_RxErrorFromISR(void) { errors_n++; }
#endif
uint32_t HAL_GetTick(void) { return milliseconds; }
void HAL_NVIC_SetPriority(unsigned a, unsigned b, unsigned c) { assert(a == TIM8_UP_TIM13_IRQn && b == 0 && c == 0); }
void HAL_NVIC_EnableIRQ(unsigned a) { assert(a == TIM8_UP_TIM13_IRQn); }
void TestSoftUart_GPIOInit(GPIO_TypeDef *port, GPIO_InitTypeDef *gpio)
{
  assert(port == GPIOF);
  if (gpio->Pin == GPIO_PIN_2) {
    assert(test_gpio.BSRR == GPIO_PIN_2); /* latch high before output enabled */
    assert(gpio->Mode == GPIO_MODE_OUTPUT_PP);
  } else { assert(gpio->Pin == GPIO_PIN_4 && gpio->Mode == GPIO_MODE_INPUT && gpio->Pull == GPIO_PULLUP); }
  gpio_calls++;
}
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *timer)
{
  if (timer == &htim8) { tim8_calls++; TIM8->SR &= ~TIM_SR_UIF; }
  else { assert(timer == &htim13); tim13_calls++; TIM13->SR &= ~TIM_SR_UIF; }
}
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *dma) { (void)dma; }
void HAL_UART_IRQHandler(UART_HandleTypeDef *uart) { (void)uart; }
void HAL_IncTick(void) { milliseconds++; }
int HAL_UART_DMAStop(UART_HandleTypeDef *uart) { (void)uart; return 0; }
int HAL_UART_Receive_DMA(UART_HandleTypeDef *uart, uint8_t *data, unsigned n) { (void)uart; (void)data; (void)n; return 0; }
void UART_RxPushFrameFromIsr(uint16_t count) { (void)count; }

static void Step(bool high)
{
  assert(test_primask == 0);
  DWT->CYCCNT += TICK_CYCLES;
  subcycles += TICK_CYCLES;
  if (subcycles >= 168000U) { subcycles -= 168000U; milliseconds++; }
  GPIOF->IDR = high ? GPIO_PIN_4 : 0;
  TIM13->CNT = 0;
  TIM13->SR |= TIM_SR_UIF;
  TIM8_UP_TIM13_IRQHandler();
  if (wave_n < sizeof(wave)) { wave[wave_n++] = GPIOF->BSRR == GPIO_PIN_2; }
}
void AppRuntime_Service(void)
{
  service_calls++;
#ifdef SOFT_UART_SHELL_WIRE
  void WireServiceHook(void);
  WireServiceHook();
#endif
  /* The real service hook cannot parse Shell; drive only interrupt ticks. */
  if (auto_drain) { for (unsigned i = 0; i < 40; i++) { Step(true); } }
  else { milliseconds++; }
}
static void Idle(unsigned count) { while (count--) { Step(true); } }
#ifndef SOFT_UART_SHELL_WIRE
static void Reset(void)
{
  if (running) { SoftUart_Stop(); }
  selected_mode = APP_MODE_DEBUG;
  GPIOF->IDR = GPIO_PIN_4;
  received_n = errors_n = wave_n = 0;
  assert(SoftUart_Init());
  assert(TIM13->PSC == 0 && TIM13->ARR == 2187);
  Idle(8);
  wave_n = 0;
}
#endif
static void SendRx(uint8_t byte, bool stop)
{
  for (unsigned t = 0; t < 80; t++) {
    bool bit = t < 8 ? false : t < 72 ? ((byte >> (t / 8 - 1)) & 1) != 0 : stop;
    Step(bit);
  }
}
#ifndef SOFT_UART_SHELL_WIRE
static void AssertFrames(unsigned count, const uint8_t *bytes)
{
  assert(wave_n >= count * 80U);
  for (unsigned n = 0; n < count; n++) {
    for (unsigned t = 0; t < 80; t++) {
      bool high = t < 8 ? false : t >= 72 ? true : ((bytes[n] >> (t / 8 - 1)) & 1) != 0;
      assert(wave[n * 80 + t] == high);
    }
  }
}
int main(void)
{
  /* HOST and clock failures must not start or reconfigure GPIO/timer. */
  selected_mode = APP_MODE_HOST;
  assert(!SoftUart_Init() && !running && gpio_calls == 0 && TIM13->DIER == 0);
  selected_mode = APP_MODE_DEBUG;
  test_hclk = 160000000U;
  assert(!SoftUart_Init() && gpio_calls == 0);
  test_hclk = 168000000U;
  DWT->CTRL = DWT_CTRL_NOCYCCNT_Msk;
  assert(!SoftUart_Init()); DWT->CTRL = 0;

  Reset();
  for (unsigned i = 0; i < 256; i++) { expected[i] = (uint8_t)i; }
  assert(SoftUart_Write(expected, 256) == 256);
  /* Full duplex: all values, contiguous frames, preserved TX/RX phase. */
  for (unsigned i = 0; i < 256; i++) { SendRx((uint8_t)i, true); }
  Idle(1);
  assert(received_n == 256 && !memcmp(received, expected, 256));
  AssertFrames(256, expected);
  assert(diagnostics.tx_bytes == 256 && diagnostics.timing_errors == 0);

  /* Independent continuous-time input, 8 start-edge phases inside a tick,
   * and +/- 2 percent peer baud error. Exercise every byte at each phase. */
  for (unsigned phase = 0; phase < 8; phase++) {
    for (unsigned bit_time = 784; bit_time <= 816; bit_time += 16) {
      Reset();
      unsigned edge = phase * 100U / 8U;
      for (unsigned time = 0; time < edge + 256U * 10U * bit_time + 900U; time += 100U) {
        bool high = true;
        if (time >= edge && time - edge < 256U * 10U * bit_time) {
          unsigned position = (time - edge) / bit_time;
          unsigned bit = position % 10U, value = position / 10U;
          high = bit == 0 ? false : bit == 9 ? true : ((value >> (bit - 1U)) & 1U) != 0;
        }
        Step(high);
      }
      assert(received_n == 256 && !memcmp(received, expected, 256));
      assert(diagnostics.frame_errors == 0 && diagnostics.timing_errors == 0);
    }
  }
  Reset();
  Step(false); Idle(5);
  assert(received_n == 0 && diagnostics.false_starts == 1);
  SendRx(0, false);
  for (unsigned i = 0; i < 800; i++) { Step(false); }
  assert(errors_n == 1 && diagnostics.frame_errors == 1 && received_n == 0);
  Idle(8); SendRx(0xA5, true);
  assert(received_n == 1 && received[0] == 0xA5);

  /* Capture input generation at START, never at STOP. */
  Reset(); generation = 3; Step(false); generation = 4;
  for (unsigned t = 1; t < 80; t++) { Step(t >= 72); }
  assert(received_n == 1 && generations[0] == 3);

  /* DWT wrap, missing ticks, lateness, frame cancellation and stable idle. */
  Reset(); DWT->CYCCNT = 0xFFFFF000U; last_cycle = DWT->CYCCNT;
  SendRx(0x55, true); assert(received_n == 1 && diagnostics.timing_errors == 0);
  assert(SoftUart_Write(expected, 4) == 4); Step(false);
  DWT->CYCCNT += TICK_CYCLES * 2U;
  Step(false);
  assert(diagnostics.timing_errors == 1 && diagnostics.tx_faults == 1 && errors_n == 1);
  assert(GPIOF->BSRR == GPIO_PIN_2 && tx_tail == tx_head && !tx_active);
  Idle(90); SendRx(0xAA, true); assert(received[received_n - 1] == 0xAA);
  DWT->CYCCNT += TICK_CYCLES; TIM13->CNT = LATE_TIMER_TICKS + 1;
  TIM13->SR |= TIM_SR_UIF; TIM8_UP_TIM13_IRQHandler();
  assert(diagnostics.timing_errors == 2);

  /* Shared IRQ: source gating, TIM8 preserved, no duplicate tick/handler. */
  Reset();
  TIM13->SR = 0; TIM8->SR = TIM_SR_UIF; TIM8->DIER = TIM_DIER_UIE;
  unsigned old8 = tim8_calls, old13 = tim13_calls;
  uint32_t cycle = last_cycle;
  TIM8_UP_TIM13_IRQHandler();
  assert(tim8_calls == old8 + 1 && tim13_calls == old13 && last_cycle == cycle);
  TIM13->DIER = 0; TIM13->SR = TIM_SR_UIF;
  TIM8_UP_TIM13_IRQHandler(); assert(last_cycle == cycle && TIM13->SR == TIM_SR_UIF);
  TIM13->DIER = TIM_DIER_UIE;
  TIM8->SR = TIM_SR_UIF; Step(true);
  assert(tim8_calls == old8 + 2 && tim13_calls == old13);
  cycle = last_cycle; TIM8_UP_TIM13_IRQHandler(); assert(last_cycle == cycle);

  /* Ring wrap/backpressure and stack data lifetime. */
  Reset();
  uint8_t temporary[16]; memcpy(temporary, expected, 16);
  assert(SoftUart_Write(temporary, 16) == 16); memset(temporary, 0xFF, 16);
  Idle(16 * 80 + 1); AssertFrames(16, expected);
  Reset(); auto_drain = true;
  for (unsigned i = 0; i < 2048; i++) { expected[i] = (uint8_t)i; }
  assert(SoftUart_Write(expected, 2048) == 2048 && service_calls > 0);
  Idle(513 * 80); AssertFrames(2048, expected);
  assert(diagnostics.tx_bytes == 2048 && diagnostics.tx_timeouts == 0);
  Reset(); auto_drain = false;
  milliseconds = 0xFFFFFF00U;
  assert(SoftUart_Write(expected, 600) == 511);
  assert(diagnostics.tx_timeouts == 1); /* no IRQ progress, bounded wait with tick wrap */
  Idle(512 * 80); assert(SoftUart_Write(expected, 1) == 1);
  test_ipsr = 1; assert(SoftUart_Write(expected, 1) == 0); test_ipsr = 0;
  test_primask = 1; assert(SoftUart_Write(expected, 1) == 0); test_primask = 0;
  SoftUart_Stop(); assert(!running && !(TIM13->CR1 & TIM_CR1_CEN) && !(TIM13->DIER & TIM_DIER_UIE));
  assert(GPIOF->BSRR == GPIO_PIN_2 && SoftUart_Write(expected, 1) == 0);
  puts("PASS software UART: all bytes, wire phases/baud, duplex, faults, wrap, IRQ sources, queues, timeout, data lifetime, HOST guard");
  return 0;
}
#endif
