#include "app_controller.h"
#include "app_config.h"
#include "app_mode.h"
#include "app_runtime.h"
#include "app_display.h"
#include "host_link.h"
#include "button.h"
#include "classification.h"
#include "pick_place.h"
#include "aux_servo.h"
#include "xy_stage.h"
#include "tray_motor.h"
#include "hmi_link.h"

static bool initialized, servicing, started, release_seen, key_down;
static bool active, pending, have_last;
static Host_PickFrame_t task, last;
static Host_Status_t last_result;
static uint8_t key_sequence, active_id;
static uint32_t hello_ms;

static bool SameTask(const Host_PickFrame_t *a, const Host_PickFrame_t *b)
{
  return a->sequence == b->sequence && a->command == b->command &&
         a->x_raw == b->x_raw && a->y_raw == b->y_raw;
}

static void OnFrame(const Host_PickFrame_t *frame)
{
  if (active) {
    (void)HostLink_SendStatus(frame->sequence, SameTask(frame, &task) ? HOST_STATUS_BUSY : HOST_STATUS_FAILED);
    return;
  }
  if (!started) { return; }
  if (have_last && SameTask(frame, &last)) {
    (void)HostLink_SendStatus(frame->sequence, last_result);
    return;
  }
  if (!HmiCode_IsValid(frame->command)) {
    (void)HostLink_SendStatus(frame->sequence, HOST_STATUS_FAILED);
    return;
  }
  if (!(frame->x_mm >= 0.0f && frame->x_mm <= XY_STAGE_X_MAX_MM &&
        frame->y_mm >= 0.0f && frame->y_mm <= XY_STAGE_Y_MAX_MM)) {
    (void)HostLink_SendStatus(frame->sequence, HOST_STATUS_OUT_OF_RANGE);
    return;
  }
  if (!XY_IsOriginValid() || !Tray_IsOriginValid() ||
      !(APP_RELEASE_X_MM >= 0.0f && APP_RELEASE_X_MM <= XY_STAGE_X_MAX_MM &&
        APP_RELEASE_Y_MM >= 0.0f && APP_RELEASE_Y_MM <= XY_STAGE_Y_MAX_MM &&
        APP_HOME_X_MM >= 0.0f && APP_HOME_X_MM <= XY_STAGE_X_MAX_MM &&
        APP_HOME_Y_MM >= 0.0f && APP_HOME_Y_MM <= XY_STAGE_Y_MAX_MM)) {
    (void)HostLink_SendStatus(frame->sequence, HOST_STATUS_FAILED);
    return;
  }
  task = *frame;
  active = true;
  pending = true;
  /* Lock before sending ACK: a lost ACK never allows the task to run twice. */
  AppRuntime_BeginTask();
  (void)HostLink_SendStatus(frame->sequence, HOST_STATUS_ACK);
  active_id = Classification_Assign(frame->command);
}

void AppController_Init(void)
{
  Classification_Init();
  if (AppMode_Get() != APP_MODE_HOST) { return; }
  Button_Init();
  HostLink_Init();
  HostLink_SetFrameHandler(OnFrame);
  (void)HMI_SendSessionReset();
  initialized = true;
  AppDisplay_SetCommand("WAIT PC");
}

uint8_t AppController_GetId(void) { return active_id; }

void AppController_Service(void)
{
  bool press;
  uint32_t now;
  if (!initialized || servicing || AppMode_Get() != APP_MODE_HOST) { return; }
  servicing = true;
  /* Drain frames before accepting this poll's button edge: queued startup
   * garbage cannot become a job merely because a fresh press just occurred. */
  HostLink_Service();
  press = Button_Poll();
  if (!release_seen) {
    release_seen = Button_IsReleasedStable();
  } else if (press && !started) {
    started = true;
    key_down = true;
    (void)HostLink_SendKeyEvent(++key_sequence, HOST_LINK_EVENT_STATE_PRESS);
    AppDisplay_SetCommand("WAIT TARGET");
  } else if (key_down && !Button_IsPressed()) {
    key_down = false;
    (void)HostLink_SendKeyEvent(++key_sequence, HOST_LINK_EVENT_STATE_RELEASE);
  }
  now = HAL_GetTick();
  if ((uint32_t)(now - hello_ms) >= 1000U) {
    hello_ms = now;
    (void)HostLink_SendHostReady();
  }
  if (!started) {
    if (!XY_IsOriginValid()) { AppDisplay_SetCommand(XY_GetDiagnosticText()); }
    else if (!Tray_IsOriginValid()) { AppDisplay_SetCommand(Tray_GetDiagnosticText()); }
    else { AppDisplay_SetCommand(HostLink_GetHostReady() ? "WAIT BUTTON" : "WAIT PC"); }
  }
  servicing = false;
}

void AppController_Poll(void)
{
  bool ok;
  Tray_Result_t tray_result;
  AppController_Service();
  if (!pending) { return; }
  pending = false;
  AppDisplay_SetCommand("TRAY POSITION");
  ok = AuxServo_LiftUp();
  if (ok) {
    tray_result = Tray_MoveToAngle(Classification_Angle(active_id));
    ok = tray_result == TRAY_RESULT_OK;
    if (!ok) { AppDisplay_SetCommand(Tray_ResultText(tray_result)); }
  }
  if (ok) { ok = PickPlace_PickAt(task.x_mm, task.y_mm, HMI_CodeShape(task.command)); }
  if (ok) { ok = PickPlace_ReleaseAt(APP_RELEASE_X_MM, APP_RELEASE_Y_MM, HMI_CodeShape(task.command)); }
  if (ok) {
    /* Exactly one append per physical release, even if parking later fails. */
    (void)HMI_SendRecord(task.command, active_id);
#if APP_PARK_AFTER_TASK
    ok = PickPlace_MoveTo(APP_HOME_X_MM, APP_HOME_Y_MM);
#endif
  }
  if (AppRuntime_Expired()) {
    /* This foreground owns the motor bus. Service hooks never send stop. */
    XY_Stop();
    Tray_Stop();
    XY_InvalidateOrigin();
    Tray_InvalidateOrigin();
    AppDisplay_SetCommand("TASK TIMEOUT");
    ok = false;
  }
  AppRuntime_EndTask();
  last = task;
  last_result = ok ? HOST_STATUS_DONE : HOST_STATUS_FAILED;
  have_last = true;
  /* Keep active set through the terminal transmission (including reentrant
   * service calls). No other task is accepted until this transaction returns. */
  (void)HostLink_SendStatus(task.sequence, last_result);
  active = false;
  if (ok) { AppDisplay_SetCommand("CYCLE DONE"); }
  /* On a motion fault keep the pump/valve state; never drop a held object. */
}
