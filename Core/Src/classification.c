#include "classification.h"
#include "app_config.h"
#include "hmi_codes.h"
#include <string.h>
static uint8_t code_to_id[HMI_CODE_MAX + 1U];
static uint8_t next_id;
void Classification_Init(void) { memset(code_to_id, 0, sizeof(code_to_id)); next_id = 1U; }
uint8_t Classification_Assign(uint8_t code)
{
  if (!HmiCode_IsValid(code)) { return 0U; }
  if (code_to_id[code] == 0U) {
    code_to_id[code] = next_id;
    next_id = (uint8_t)(next_id % APP_CLASSIFICATION_SLOTS + 1U);
  }
  return code_to_id[code];
}
float Classification_Angle(uint8_t id) { return (float)(id - 1U) * 60.0f; }
