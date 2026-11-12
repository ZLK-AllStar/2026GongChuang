#include "shell.h"
#include "shell_port.h"
#include "app_mode.h"
#include "app_display.h"
#include "xy_stage.h"
#include "tray_motor.h"
#include "aux_servo.h"
#include "vacuum.h"
#include "hmi_link.h"
#include "host_link.h"
#include "soft_uart.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern void shellUp(Shell *), shellDown(Shell *), shellRight(Shell *), shellLeft(Shell *);
extern void shellTab(Shell *), shellBackspace(Shell *), shellDelete(Shell *), shellEnter(Shell *);
extern void shellHelp(int argc, char *argv[]);

static int Error(const char *reason)
{
  shellPrint(shellGetCurrent(), "ERR %s\r\n", reason);
  return -1;
}

static bool Number(const char *text, float max, float *value)
{
  char *end;
  double number;
  errno = 0;
  number = strtod(text, &end);
  if (!*text || end == text || *end || errno || !(number >= 0.0 && number <= max)) { return false; }
  *value = (float)number;
  return true;
}

static int Move(int argc, char *argv[])
{
  float x, y;
  XY_Result_t result;
  if (argc != 3 || !Number(argv[1], XY_STAGE_X_MAX_MM, &x) || !Number(argv[2], XY_STAGE_Y_MAX_MM, &y)) {
    return Error("move usage: move <0..215 mm> <0..205 mm>");
  }
  if (!XY_IsOriginValid()) { return Error(XY_GetDiagnosticText()); }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  AppDisplay_SetMotionTarget(x, y);
  result = AuxServo_LiftUp() ? XY_MoveTo(x, y) : XY_RESULT_TIMEOUT;
  shellPrint(shellGetCurrent(), "%s XY result=%u pos=%.3f,%.3f %s\r\n",
             result == XY_RESULT_OK ? "OK" : "ERR", result, XY_GetX(), XY_GetY(), XY_ResultText(result));
  ShellPort_EndAction();
  return result == XY_RESULT_OK ? 0 : -1;
}

static int Lift(int argc, char *argv[])
{
  const char *usage = "lift usage: lift up|down|full|<0..270 deg integer>";
  char *end;
  long angle;
  bool ok;
  if (argc != 2) { return Error(usage); }
  if (!strcmp(argv[1], "up")) { angle = AUX_SERVO_LIFT_UP_ANGLE; }
  else if (!strcmp(argv[1], "down")) { angle = AUX_SERVO_LIFT_DOWN_ANGLE; }
  else if (!strcmp(argv[1], "full")) { angle = AUX_SERVO_LIFT_DOWN_ANGLE_FULL; }
  else {
    errno = 0;
    angle = strtol(argv[1], &end, 10);
    if (!*argv[1] || end == argv[1] || *end || errno ||
        angle < (long)AUX_SERVO_MIN_ANGLE || angle > (long)AUX_SERVO_MAX_ANGLE) {
      return Error(usage);
    }
  }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  ok = AuxServo_LiftTo((uint16_t)angle);
  shellPrint(shellGetCurrent(), "%s LIFT angle=%u\r\n", ok ? "OK" : "ERR", AuxServo_GetLiftAngle());
  ShellPort_EndAction();
  return ok ? 0 : -1;
}

static int Tray(int argc, char *argv[])
{
  float angle;
  Tray_Result_t result;
  if (argc != 2 || !Number(argv[1], 360.0f, &angle)) { return Error("tray usage: tray <0..360 deg>"); }
  if (!Tray_IsOriginValid()) { return Error(Tray_GetDiagnosticText()); }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  result = Tray_MoveToAngle(angle);
  shellPrint(shellGetCurrent(), "%s TRAY %s phase=%.3f accumulated=%.3f\r\n",
             result == TRAY_RESULT_OK ? "OK" : "ERR", Tray_ResultText(result),
             Tray_GetAngleDeg(), Tray_GetAccumulatedAngleDeg());
  ShellPort_EndAction();
  return result == TRAY_RESULT_OK ? 0 : -1;
}

static int Vacuum(int argc, char *argv[])
{
  bool ok = true;
  if (argc != 2 || (strcmp(argv[1], "on") && strcmp(argv[1], "off"))) { return Error("vacuum usage: vacuum on|off"); }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  if (!strcmp(argv[1], "on")) { Vacuum_SetValve(false); Vacuum_SetPump(true); }
  else { ok = Vacuum_Release(); }
  shellPrint(shellGetCurrent(), "%s VACUUM pump=%u valve=%u\r\n", ok ? "OK" : "ERR", Vacuum_IsPumpOn(), Vacuum_IsValveOn());
  ShellPort_EndAction();
  return ok ? 0 : -1;
}

static int Hmi(int argc, char *argv[])
{
  char *end;
  long code;
  bool ok;
  if (argc != 2) { return Error("hmi usage: hmi <1..90 decimal>"); }
  errno = 0;
  code = strtol(argv[1], &end, 10);
  if (!*argv[1] || *end || errno || code < HMI_LINK_CODE_MIN || code > HMI_LINK_CODE_MAX) { return Error("HMI code range/integer"); }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  ok = HMI_SendCode((uint8_t)code);
  shellPrint(shellGetCurrent(), "%s HMI preview code=%ld (local TX only)\r\n", ok ? "OK" : "ERR", code);
  ShellPort_EndAction();
  return ok ? 0 : -1;
}

static int Status(int argc, char *argv[])
{
  SoftUart_Diagnostics uart;
  (void)argv;
  if (argc != 1) { return Error("status takes no parameters"); }
  if (AppMode_Get() != APP_MODE_DEBUG) { return Error("MODE"); }
  shellPrint(shellGetCurrent(), "OK mode=%s XY origin=%u pos=%.3f,%.3f %s\r\n", AppMode_Name(), XY_IsOriginValid(), XY_GetX(), XY_GetY(), XY_GetDiagnosticText());
  shellPrint(shellGetCurrent(), "XY target=%.3f,%.3f\r\n", XY_GetTargetX(), XY_GetTargetY());
  shellPrint(shellGetCurrent(), "TRAY origin=%u phase=%.3f accum=%.3f %s\r\n", Tray_IsOriginValid(), Tray_GetAngleDeg(), Tray_GetAccumulatedAngleDeg(), Tray_GetDiagnosticText());
  shellPrint(shellGetCurrent(), "pump=%u valve=%u lift=%u errors HOST=%lu/%lu HMI=%lu SHELL=%lu\r\n", Vacuum_IsPumpOn(), Vacuum_IsValveOn(), AuxServo_GetLiftAngle(), (unsigned long)HostLink_GetErrorCount(), (unsigned long)HostLink_GetOverflowCount(), (unsigned long)HMI_GetErrorCount(), (unsigned long)ShellPort_GetErrorCount());
  SoftUart_GetDiagnostics(&uart);
  shellPrint(shellGetCurrent(), "SOFTUART rx=%lu tx=%lu frame=%lu false=%lu\r\n",
             (unsigned long)uart.rx_bytes, (unsigned long)uart.tx_bytes,
             (unsigned long)uart.frame_errors, (unsigned long)uart.false_starts);
  shellPrint(shellGetCurrent(), "SOFTUART timing=%lu txfault=%lu timeout=%lu\r\n",
             (unsigned long)uart.timing_errors, (unsigned long)uart.tx_faults,
             (unsigned long)uart.tx_timeouts);
  shellPrint(shellGetCurrent(), "SOFTUART maxgap=%lu CPU cycles maxlate=%lu TIM13 ticks (diagnostic, not full IRQ latency)\r\n",
             (unsigned long)uart.max_gap_cycles, (unsigned long)uart.max_late_timer_ticks);
  return 0;
}

static int Zero(int argc, char *argv[])
{
  int result;
  if (argc != 2 || (strcmp(argv[1], "xy") && strcmp(argv[1], "tray"))) { return Error("zero usage: zero xy|tray (CURRENT position)"); }
  if (!ShellPort_BeginAction()) { return Error("MODE/BUSY"); }
  if (!strcmp(argv[1], "xy")) {
    XY_Stop();
    result = (int)XY_Init();
    if (!result) { result = (int)XY_SetZero(); }
    shellPrint(shellGetCurrent(), "%s XY software origin at current position: %s\r\n", result ? "ERR" : "OK", XY_GetDiagnosticText());
  } else {
    Tray_Stop();
    result = (int)Tray_Init();
    shellPrint(shellGetCurrent(), "%s TRAY software origin at current position: %s\r\n", result ? "ERR" : "OK", Tray_GetDiagnosticText());
  }
  ShellPort_EndAction();
  return result ? -1 : 0;
}

static int Help(int argc, char *argv[])
{
  if (argc > 2) { return Error("help [command]"); }
  shellHelp(argc, argv);
  return 0;
}

#define COMMAND(name, func, desc) SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, name, func, desc)
#define KEY(value, func) SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED, value, func, key)
const ShellCommand shellCommandList[] = {
  { .attr.value = SHELL_CMD_TYPE(SHELL_TYPE_USER), .data.user.name = SHELL_DEFAULT_USER,
    .data.user.password = "", .data.user.desc = "DEBUG console" },
  KEY(0x1B5B4100, shellUp), KEY(0x1B5B4200, shellDown),
  KEY(0x1B5B4300, shellRight), KEY(0x1B5B4400, shellLeft),
  KEY(0x09000000, shellTab), KEY(0x08000000, shellBackspace),
  KEY(0x7F000000, shellBackspace), KEY(0x1B5B337E, shellDelete),
  KEY(0x0A000000, shellEnter), KEY(0x0D000000, shellEnter),
  COMMAND(help, Help, help [command]),
  COMMAND(move, Move, move x_mm y_mm: 0..215 / 0..205; example move 1 0),
  COMMAND(lift, Lift, lift up|down|full|angle_deg: integer 0..270; example lift 120),
  COMMAND(tray, Tray, tray angle_deg: 0..360; forward phase; example tray 60),
  COMMAND(vacuum, Vacuum, vacuum on|off; off releases pressure),
  COMMAND(hmi, Hmi, hmi code: 1..90 decimal; preview only),
  COMMAND(status, Status, cached status and errors),
  COMMAND(zero, Zero, zero xy|tray: CURRENT location becomes software origin)
};
const unsigned short shellCommandCount = sizeof(shellCommandList) / sizeof(shellCommandList[0]);
