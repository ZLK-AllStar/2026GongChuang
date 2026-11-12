#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Core/Src/shell_commands.c"
#include "../../Core/Src/shell_port.c"
static unsigned actions, previews, zeroes;
static uint16_t commanded_lift_angle;
static bool fail_lift;
static App_Mode_t selected_mode=APP_MODE_DEBUG;
static char output[32768];
static size_t output_n;
static bool inject_busy;
static bool rx_active, backend_ready, inject_error, inject_poll, short_write;
static unsigned backend_inits;
static void Input(const char *text);
uint32_t HAL_GetTick(void) { static uint32_t tick; return tick++; }
bool SoftUart_Init(void) { backend_inits++; backend_ready=true; return true; }
bool SoftUart_IsRxActive(void) { return rx_active; }
void SoftUart_GetDiagnostics(SoftUart_Diagnostics *d) { memset(d,0,sizeof(*d)); }
uint16_t SoftUart_Write(const uint8_t *p,uint16_t n) {
  assert(backend_ready); /* shellInit must not print before TX exists. */
  if(inject_poll) { unsigned before=actions;ShellPort_Poll();assert(actions==before); }
  if(inject_error) { inject_error=false;ShellPort_RxErrorFromISR(); }
  if(output_n+n<sizeof(output)) { memcpy(output+output_n,p,n);output_n+=n;output[output_n]=0; }
  return short_write ? n/2 : n;
}
App_Mode_t AppMode_Get(void) { return selected_mode; }
const char *AppMode_Name(void) { return "DEBUG"; }
bool XY_IsOriginValid(void) { return true; }
bool Tray_IsOriginValid(void) { return true; }
const char *XY_GetDiagnosticText(void) { return "OK"; }
const char *Tray_GetDiagnosticText(void) { return "OK"; }
const char *Tray_ResultText(Tray_Result_t r) { (void)r;return "OK"; }
float XY_GetX(void) { return 0; } float XY_GetY(void) { return 0; }
float XY_GetTargetX(void) { return 0; } float XY_GetTargetY(void) { return 0; }
const char *XY_ResultText(XY_Result_t r) { (void)r;return "OK"; }
float Tray_GetAngleDeg(void) { return 0; } float Tray_GetAccumulatedAngleDeg(void) { return 0; }
uint16_t AuxServo_GetLiftAngle(void) { return 240; }
bool Vacuum_IsPumpOn(void) { return false; } bool Vacuum_IsValveOn(void) { return false; }
uint32_t HostLink_GetErrorCount(void) { return 0; } uint32_t HostLink_GetOverflowCount(void) { return 0; }
uint32_t HMI_GetErrorCount(void) { return 0; }
void AppDisplay_SetMotionTarget(float x,float y) { (void)x;(void)y; }
bool AuxServo_LiftUp(void) { return true; }
bool AuxServo_LiftDown(void) { actions++;return true; }
bool AuxServo_LiftDownFull(void) { actions++;return true; }
bool AuxServo_LiftTo(uint16_t angle) { commanded_lift_angle=angle;actions++;return !fail_lift; }
void Vacuum_SetValve(bool on) { (void)on;actions++; }
void Vacuum_SetPump(bool on) { (void)on;actions++; }
bool Vacuum_Release(void) { actions++;return true; }
XY_Result_t XY_MoveTo(float x,float y) {
  assert(x>=0 && y>=0); actions++;
  if(inject_busy) Input("tray 60\r");
  return XY_RESULT_OK;
}
Tray_Result_t Tray_MoveToAngle(float a) { assert(a>=0 && a<=360);actions++;return TRAY_RESULT_OK; }
void XY_Stop(void) {} void Tray_Stop(void) {}
XY_Result_t XY_Init(void) { return XY_RESULT_OK; }
XY_Result_t XY_SetZero(void) { zeroes++;return XY_RESULT_OK; }
Tray_Result_t Tray_Init(void) { zeroes++;return TRAY_RESULT_OK; }
bool HMI_SendCode(uint8_t code) { assert(code>=1 && code<=90);previews++;return true; }
static void Input(const char *text) { while(*text) { ShellPort_RxByteFromISR((uint8_t)*text++,ShellPort_RxGenerationFromISR()); } }
static void Run(const char *line) { Input(line);ShellPort_Poll(); }
int main(void) {
  unsigned before;
  selected_mode=APP_MODE_HOST;ShellPort_Init();assert(backend_inits==0);
  selected_mode=APP_MODE_DEBUG;
  ShellPort_Init();
  Run("move 1 0\r");if(actions!=1) fprintf(stderr,"%s\n",output);assert(actions==1);
  Run("tray 360\r");assert(actions==2);
  Run("lift full\r");assert(actions==3);
  Run("vacuum off\r");assert(actions==4);
  Run("hmi 72\r");Run("hmi 73\r");Run("hmi 80\r");Run("hmi 81\r");Run("hmi 90\r");assert(previews==5);
  before=actions;
  Run("lift up\r");assert(actions==++before && commanded_lift_angle==AUX_SERVO_LIFT_UP_ANGLE);
  Run("lift down\r");assert(actions==++before && commanded_lift_angle==AUX_SERVO_LIFT_DOWN_ANGLE);
  Run("lift full\r");assert(actions==++before && commanded_lift_angle==AUX_SERVO_LIFT_DOWN_ANGLE_FULL);
  Run("lift 0\r");assert(actions==++before && commanded_lift_angle==0);
  Run("lift 120\r");assert(actions==++before && commanded_lift_angle==120);
  Run("lift 270\r");assert(actions==++before && commanded_lift_angle==270);
  fail_lift=true;Run("lift 90\r");fail_lift=false;
  assert(actions==++before && strstr(output,"ERR LIFT"));
  Run("lift up\r");assert(actions==++before); /* Failure must release busy. */
  before=actions;
  Run("move 215 205\r");assert(actions==++before);
  Run("move nan 0\r");Run("move inf 0\r");Run("move -1 0\r");Run("move 215.01 0\r");Run("move 216 0\r");
  Run("move 0 205.01\r");Run("move 0 206\r");Run("move 0 225\r");
  Run("move 1x 0\r");Run("move 1\r");Run("move 1 0 extra\r");
  Run("tray 361\r");Run("tray nan\r");Run("lift sideways\r");Run("vacuum off extra\r");
  Run("lift\r");Run("lift 120 extra\r");Run("lift -1\r");Run("lift 271\r");
  Run("lift 90.5\r");Run("lift nan\r");Run("lift inf\r");Run("lift 120x\r");
  Run("lift 0x10\r");Run("lift 1e2\r");Run("lift 999999999999999999999999\r");
  Run("hmi 1.0\r");Run("hmi 0x10\r");Run("hmi 91\r");
  assert(actions==before && previews==5);
  Run("zero xy\r");Run("zero tray\r");assert(zeroes==2);
  // A valid command prefix followed by an overlong tail must never execute.
  Input("move 1 0");for(unsigned i=0;i<100;i++) Input(" ");Input("\r");ShellPort_Poll();
  assert(actions==before);
  Run("move 1 0\r");assert(actions==++before);
  inject_busy=true;Run("move 1 0\r");assert(actions==++before);inject_busy=false;
  ShellPort_Poll();assert(actions==before); // accumulated command was discarded
  for(unsigned i=0;i<400;i++) { Input("x"); }
  ShellPort_Poll();
  Run("\r");Run("tray 60\r");assert(actions==++before);
  ShellPort_RxErrorFromISR();ShellPort_Poll();Run("\r");Run("tray 60\r");assert(actions==++before);
  // A receive error during echo must be noticed again before executing Enter.
  inject_error=true;Run("tray 60\r");assert(actions==before);
  Run("\r");Run("tray 60\r");assert(actions==++before);
  // Unit-level token rejection at both busy boundaries; real bit positions
  // are covered by test_shell_wire.c with the production state machine.
  {
    uint32_t token=ShellPort_RxGenerationFromISR();
    rx_active=true;assert(ShellPort_BeginAction());
    ShellPort_EndAction();rx_active=false;
    ShellPort_RxByteFromISR('t',token);Input("ray 60\r");ShellPort_Poll();
    assert(actions==before);Run("\r");Run("tray 60\r");assert(actions==++before);
    assert(ShellPort_BeginAction());token=ShellPort_RxGenerationFromISR();
    rx_active=true;ShellPort_EndAction();rx_active=false;
    ShellPort_RxByteFromISR('t',token);Input("ray 60\r");ShellPort_Poll();
    assert(actions==before);Run("\r");Run("tray 60\r");assert(actions==++before);
  }
  inject_poll=true;Run("help\r");Run("status\r");inject_poll=false;assert(actions==before);
  uint32_t old_errors=ShellPort_GetErrorCount();short_write=true;Run("status\r");short_write=false;
  assert(ShellPort_GetErrorCount()>old_errors);Run("tray 60\r");assert(actions==++before);
  selected_mode=APP_MODE_HOST;
  Run("move 1 0\r");assert(actions==before);
  assert(!ShellPort_BeginAction());
  { char *args[]={"lift","120"};assert(Lift(2,args)==-1 && actions==before); }
  selected_mode=APP_MODE_DEBUG;
  assert(ShellPort_BeginAction());
  { char *args[]={"lift","120"};assert(Lift(2,args)==-1 && actions==before); }
  ShellPort_EndAction();
  assert(strstr(output,"ERR INPUT") && strstr(output,"line too long"));
  puts("PASS Shell: actual library/commands/port, lift presets/angles/errors, validation, damaged lines, in-flight busy input, short writes, no reentry, mode guard");
  return 0;
}
