/* Real GPIO/timer state machines -> real port -> actual Letter Shell. */
#define SOFT_UART_SHELL_WIRE
#include "test_soft_uart.c"
#include "../../Core/Src/shell_port.c"

static unsigned executed;
static bool inject_overflow, test_reentry;
static int TrayCommand(int argc, char **argv)
{
  assert(argc == 2 && !strcmp(argv[1], "60"));
  assert(ShellPort_BeginAction());
  executed++;
  ShellPort_EndAction();
  return 0;
}
static int FloodCommand(int argc, char **argv)
{
  (void)argc; (void)argv;
  for (unsigned i = 0; i < 80; i++) {
    shellPrint(&console, "Long help-style output: 0123456789 ABCDEFG\r\n");
  }
  return 0;
}
extern void shellEnter(Shell *shell);
const ShellCommand shellCommandList[] = {
  { .attr.value = SHELL_CMD_TYPE(SHELL_TYPE_USER), .data.user.name = SHELL_DEFAULT_USER,
    .data.user.password = "", .data.user.desc = "test" },
  SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED, 0x0D000000, shellEnter, enter),
  SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, tray, TrayCommand, test),
  SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, flood, FloodCommand, test)
};
const unsigned short shellCommandCount = sizeof(shellCommandList) / sizeof(shellCommandList[0]);

static void SendText(const char *text)
{
  while (*text) { SendRx((uint8_t)*text++, true); }
}
static void RunLine(const char *line) { SendText(line); ShellPort_Poll(); Idle(60000); }
void WireServiceHook(void)
{
  if (test_reentry) {
    unsigned before = executed;
    ShellPort_Poll();
    assert(executed == before);
  }
  if (inject_overflow) {
    inject_overflow = false;
    SendText("tray 60\r");
    for (unsigned i = 0; i < 400; i++) { SendRx('x', true); }
  }
}
static void PartOfT(unsigned begin, unsigned end)
{
  for (unsigned t = begin; t < end; t++) {
    Step(t < 8 ? false : t < 72 ? (('t' >> (t / 8 - 1)) & 1) != 0 : true);
  }
}
int main(void)
{
  GPIOF->IDR = GPIO_PIN_4;
  selected_mode = APP_MODE_HOST; ShellPort_Init();
  assert(!running && !initialized && gpio_calls == 0);
  selected_mode = APP_MODE_DEBUG; auto_drain = true;
  ShellPort_Init(); Idle(60000);
  assert(running && initialized && !diagnostics.tx_timeouts);
  RunLine("tray 60\r"); assert(executed == 1);
  /* Action begins/ends in an actual start, data or stop bit, before the
   * stop sample. RX phase must survive and its remaining line must not run. */
  unsigned phases[] = {1, 28, 73, 76};
  for (unsigned i = 0; i < sizeof(phases) / sizeof(phases[0]); i++) {
    for (unsigned end_in_flight = 0; end_in_flight < 2; end_in_flight++) {
      unsigned before = executed;
      if (end_in_flight) { assert(ShellPort_BeginAction()); }
      PartOfT(0, phases[i]);
      assert(SoftUart_IsRxActive());
      if (!end_in_flight) { assert(ShellPort_BeginAction()); }
      ShellPort_EndAction();
      PartOfT(phases[i], 80);
      RunLine("ray 60\r"); assert(executed == before);
      RunLine("\r"); RunLine("tray 60\r"); assert(executed == before + 1);
    }
  }
  unsigned before = executed;
  SendText("tray "); ShellPort_Poll();
  SendRx('6', false); Idle(16); RunLine("0\r"); assert(executed == before);
  RunLine("\r"); RunLine("tray 60\r"); assert(executed == ++before);
  SendText("tray "); ShellPort_Poll();
  DWT->CYCCNT += TICK_CYCLES * 2; Idle(16);
  RunLine("60\r"); assert(executed == before);
  RunLine("\r"); RunLine("tray 60\r"); assert(executed == ++before);

  inject_overflow = test_reentry = true;
  RunLine("flood\r"); test_reentry = false;
  assert(!inject_overflow && executed == before);
  RunLine("\r"); RunLine("tray 60\r"); assert(executed == ++before);
  assert(diagnostics.frame_errors == 1 && diagnostics.timing_errors == 1);
  assert(!diagnostics.tx_timeouts && ShellPort_GetErrorCount() > 0);
  puts("PASS wire-to-Shell: real timer/driver/port/library, busy in start/data/stop, frame loss, overflow inside long output, recovery, no reentry");
  return 0;
}
