#ifndef HMI_CODES_H
#define HMI_CODES_H
#include <stdint.h>

/* Keep codes 1..80 stable; append the cone's ten colors at 81..90. */
#define HMI_CODE_MIN 1U
#define HMI_LEGACY_SHAPE_COUNT 8U
#define HMI_SHAPE_COUNT 9U
#define HMI_LEGACY_COLOR_COUNT 9U
#define HMI_COLOR_COUNT 10U
#define HMI_WHITE_COLOR 9U
#define HMI_WHITE_CODE_MIN 73U
#define HMI_CONE_SHAPE 8U
#define HMI_CONE_CODE_MIN 81U
#define HMI_CODE_MAX 90U

static inline int HmiCode_IsValid(unsigned code)
{
  return code >= HMI_CODE_MIN && code <= HMI_CODE_MAX;
}

static inline uint8_t HmiCode_Make(unsigned shape, unsigned color)
{
  if (shape >= HMI_SHAPE_COUNT || color >= HMI_COLOR_COUNT) { return 0U; }
  if (shape == HMI_CONE_SHAPE) { return (uint8_t)(HMI_CONE_CODE_MIN + color); }
  return (uint8_t)(color == HMI_WHITE_COLOR ? HMI_WHITE_CODE_MIN + shape :
                   shape * HMI_LEGACY_COLOR_COUNT + color + HMI_CODE_MIN);
}

static inline uint8_t HmiCode_Shape(unsigned code)
{
  if (!HmiCode_IsValid(code)) { return 0U; }
  if (code >= HMI_CONE_CODE_MIN) { return HMI_CONE_SHAPE; }
  return (uint8_t)(code >= HMI_WHITE_CODE_MIN ? code - HMI_WHITE_CODE_MIN :
                   (code - HMI_CODE_MIN) / HMI_LEGACY_COLOR_COUNT);
}

static inline uint8_t HmiCode_Color(unsigned code)
{
  if (!HmiCode_IsValid(code)) { return 0U; }
  if (code >= HMI_CONE_CODE_MIN) { return (uint8_t)(code - HMI_CONE_CODE_MIN); }
  return (uint8_t)(code >= HMI_WHITE_CODE_MIN ? HMI_WHITE_COLOR :
                   (code - HMI_CODE_MIN) % HMI_LEGACY_COLOR_COUNT);
}
#endif
