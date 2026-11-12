/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pick_place.h
  * @brief   Reusable pick and release operations at an absolute XY target.
  *
*          Both operations follow the sequence configured for this machine:
 *          raise the cup, move to the target, lower the cup, run the vacuum
 *          action, then lift the cup back up.  The caller supplies the target
 *          coordinates and the material shape: the shape selects the pick
 *          depth, while a release descends to the tuned release angle.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __PICK_PLACE_H__
#define __PICK_PLACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "hmi_link.h"
#include <stdbool.h>

/* Settling time allowed after an XY move completes, before moving the cup. */
#define PICK_PLACE_MOVE_SETTLE_MS   100U

/* Pick: move to (x_mm, y_mm), lower the cup, pull vacuum and hold, lift up.
 * The shape selects how far the cup descends (see PickPlace_LiftDownForShape).
 * Returns false when the move failed; the display then carries the reason. */
bool PickPlace_PickAt(float x_mm, float y_mm, HMI_Shape_t shape);

/* Release: move to (x_mm, y_mm), lower the cup to the release angle
 * (AUX_SERVO_RELEASE_ANGLE), drop the item, lift up. Returns false when the move
 * failed; the display then carries the reason. The shape is accepted for
 * symmetry with the pick and is currently unused. */
bool PickPlace_ReleaseAt(float x_mm, float y_mm, HMI_Shape_t shape);

/* Raise the cup if it is still down, then move the stage to (x_mm, y_mm) —
 * no vacuum action. Use it to park the stage (e.g. back to the origin) between
 * or after pick/release operations. Returns false on a failed move. */
bool PickPlace_MoveTo(float x_mm, float y_mm);

#ifdef __cplusplus
}
#endif

#endif /* __PICK_PLACE_H__ */
