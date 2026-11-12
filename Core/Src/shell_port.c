#include "shell_port.h"
#include "shell.h"
#include "app_config.h"
#include "app_mode.h"
#include "main.h"
#include "soft_uart.h"
static Shell console;
static char edit_buffer[APP_SHELL_BUFFER_SIZE];
static volatile uint8_t ring[APP_SHELL_RING_SIZE];
static volatile uint16_t head, tail;
static volatile bool busy, dropped, line_open, damaged;
/* Separate ISR/foreground writers so an interrupt cannot lose an increment. */
static volatile uint32_t rx_errors;
static uint32_t errors;
static volatile uint32_t input_generation;
static volatile bool initialized;
static bool polling, discard_line;

static short Write(char *data, unsigned short length)
{
  uint16_t accepted = SoftUart_Write((const uint8_t *)data, length);
  if (accepted != length) { errors++; }
  return (short)accepted;
}

void ShellPort_Init(void)
{
  if (AppMode_Get() != APP_MODE_DEBUG || initialized) { return; }
  head = tail = 0U;
  busy = dropped = line_open = damaged = false;
  discard_line = false;
  input_generation = 0U;
  console.write = Write;
  /* shellInit prints a prompt: the interrupt-driven TX must already work.
   * RX callbacks ignore input until the entire Shell object is ready. */
  if (!SoftUart_Init()) { errors++; return; }
  shellInit(&console, edit_buffer, sizeof(edit_buffer));
  {
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();
    input_generation++; /* Also invalidate frames started during shellInit. */
    initialized = true;
    __set_PRIMASK(irq_state);
  }
  shellPrint(&console, "\r\nDEBUG PF2 TX/PF4 RX %u 8N1: current position is origin; help for commands. Line max %u bytes.\r\n",
             (unsigned)APP_SHELL_SOFT_UART_BAUD,
             (unsigned)(console.parser.bufferSize - 1U));
}

uint32_t ShellPort_RxGenerationFromISR(void) { return input_generation; }

void ShellPort_RxByteFromISR(uint8_t byte, uint32_t generation)
{
  uint16_t next;
  if (!initialized) { return; }
  line_open = byte != '\r' && byte != '\n';
  next = (uint16_t)((head + 1U) % APP_SHELL_RING_SIZE);
  if (busy || generation != input_generation) {
    dropped = true;
    /* A frame started before/inside an action but completed after it. Flush
     * its entire residual line even if the foreground already resumed. */
    if (!busy) { damaged = true; }
  }
  else if (next == tail) { rx_errors++; damaged = true; }
  else { ring[head] = byte; head = next; }
}

void ShellPort_RxErrorFromISR(void)
{
  if (!initialized) { return; }
  rx_errors++;
  damaged = true;
}

bool ShellPort_BeginAction(void)
{
  uint32_t irq_state;
  if (AppMode_Get() != APP_MODE_DEBUG || busy) { return false; }
  irq_state = __get_PRIMASK();
  __disable_irq();
  input_generation++;
  busy = true;
  __set_PRIMASK(irq_state);
  return true;
}

void ShellPort_EndAction(void)
{
  uint32_t irq_state = __get_PRIMASK();
  bool queued, lost;
  /* Generation changes invalidate in-flight RX without resetting TX phase. */
  __disable_irq();
  queued = head != tail;
  tail = head;
  discard_line = line_open || SoftUart_IsRxActive();
  lost = dropped;
  dropped = false;
  input_generation++;
  busy = false;
  __set_PRIMASK(irq_state);
  if (queued || lost || discard_line) {
    shellPrint(&console, "\r\nERR INPUT discarded while busy; retype after prompt\r\n");
  }
}

uint32_t ShellPort_GetErrorCount(void) { return errors + rx_errors; }

void ShellPort_Poll(void)
{
  unsigned count;
  if (!initialized || polling || AppMode_Get() != APP_MODE_DEBUG) { return; }
  polling = true;
  for (count = 0U; count < APP_SHELL_RING_SIZE; count++) {
    uint32_t irq_state = __get_PRIMASK();
    bool broken, have_byte;
    char c = 0;
    /* Recheck damage on EVERY byte: echo/help can block long enough for an
     * ISR error/overflow to occur inside this same Poll invocation. */
    __disable_irq();
    broken = damaged;
    if (broken) { tail = head; damaged = false; }
    have_byte = tail != head;
    if (have_byte) {
      c = (char)ring[tail];
      tail = (uint16_t)((tail + 1U) % APP_SHELL_RING_SIZE);
    }
    __set_PRIMASK(irq_state);
    if (broken) {
      console.parser.length = console.parser.cursor = console.parser.keyValue = 0;
      console.parser.buffer[0] = 0;
      discard_line = true;
      shellPrint(&console, "\r\nERR INPUT damaged/overflow/busy frame; press Enter and retype\r\n");
      continue;
    }
    if (!have_byte) { break; }
    if (discard_line) {
      if (c == '\r' || c == '\n') { discard_line = false; }
      continue;
    }
    if ((unsigned char)c >= 32U && c != 127 &&
        console.parser.length >= console.parser.bufferSize - 1U) {
      console.parser.length = console.parser.cursor = console.parser.keyValue = 0;
      console.parser.buffer[0] = 0;
      discard_line = true;
      errors++;
      shellPrint(&console, "\r\nERR INPUT line too long; whole line discarded\r\n");
      continue;
    }
    shellHandler(&console, c);
  }
  polling = false;
}
