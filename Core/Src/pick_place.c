/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pick_place.c
  * @brief   Reusable pick and release operations at an absolute XY target.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "pick_place.h"
#include "xy_stage.h"
#include "vacuum.h"
#include "aux_servo.h"
#include "app_display.h"
#include "app_runtime.h"

static bool PickPlace_GoTo(float x_mm, float y_mm, const char *state);
static bool PickPlace_LowerCup(HMI_Shape_t shape);
static const char *PickPlace_XYResultText(XY_Result_t result);

bool PickPlace_MoveTo(float x_mm, float y_mm)
{
  return PickPlace_GoTo(x_mm, y_mm, "MOVE");
}

bool PickPlace_PickAt(float x_mm, float y_mm, HMI_Shape_t shape)
{
  if (!PickPlace_GoTo(x_mm, y_mm, "TO PICK"))
  {
    return false;
  }

  if (!PickPlace_LowerCup(shape)) { return false; }
  AppDisplay_SetCommand("PICK");
  if (!Vacuum_PickAndHold()) { return false; }
  AppDisplay_SetCommand("LIFT");
  if (!AuxServo_LiftUp()) { return false; }

  return true;
}

bool PickPlace_ReleaseAt(float x_mm, float y_mm, HMI_Shape_t shape)
{
  /* Every material drops from the same release angle: the part hangs below the
   * cup, so it does not need the extra travel a pick depth provides. */
  (void)shape;

  if (!PickPlace_GoTo(x_mm, y_mm, "TO PLACE"))
  {
    return false;
  }

  AppDisplay_SetCommand("RELEASE");
  if (!Vacuum_Release()) { return false; }
  AppDisplay_SetCommand("LIFT");
  if (!AuxServo_LiftUp()) { return false; }

  return true;
}

/* Select pickup depth by protocol shape, independent of the material color. */
static bool PickPlace_LowerCup(HMI_Shape_t shape)
{
  if (shape == HMI_SHAPE_CUBOID)
  {
    AppDisplay_SetCommand("LOWER FULL");
    return AuxServo_LiftDownFull();
  }
  else if (shape == HMI_SHAPE_CYLINDER)
  {
    AppDisplay_SetCommand("LOWER CYLINDER");
    return AuxServo_LiftTo(AUX_SERVO_LIFT_DOWN_ANGLE_CYLINDER);
  }
  else
  {
    AppDisplay_SetCommand("LOWER");
    return AuxServo_LiftDown();
  }
}

static bool PickPlace_GoTo(float x_mm, float y_mm, const char *state)
{
  XY_Result_t result;

  AppDisplay_SetCommand(state);
  /* Travel only with the cup raised: the mechanics park the cup down at
   * power-on and a lowered cup would drag across the table. No-op once up. */
  if (!AuxServo_LiftUp()) { return false; }
  AppDisplay_SetMotionTarget(x_mm, y_mm);
  result = XY_MoveTo(x_mm, y_mm);
  if (result != XY_RESULT_OK)
  {
    AppDisplay_SetCommand(PickPlace_XYResultText(result));
    return false;
  }
  return AppRuntime_Wait(PICK_PLACE_MOVE_SETTLE_MS);
}

static const char *PickPlace_XYResultText(XY_Result_t result)
{
  switch (result)
  {
    case XY_RESULT_TIMEOUT:
      return "XY TIMEOUT";
    case XY_RESULT_CMD_ERROR:
      return "XY CMD ERR";
    case XY_RESULT_MOTOR_FAULT:
      return "MOTOR FAULT";
    case XY_RESULT_POSITION_READ_ERROR:
      return "POS READ ERR";
    case XY_RESULT_OUT_OF_RANGE:
      return "XY RANGE ERR";
    case XY_RESULT_ORIGIN_ERROR: return XY_GetDiagnosticText();
    case XY_RESULT_OK: return "OK";
    default: return "XY UNKNOWN ERR";
  }
}
