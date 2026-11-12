#ifndef __EXT_SRAM_H__
#define __EXT_SRAM_H__

#include "main.h"

#define TFT_BUFFER_HEIGHT   (240UL / 2UL)
#define TFT_FRAMEBUFFER_LEN (320UL * TFT_BUFFER_HEIGHT)

/* Kept as a source-compatible name for older application code. */
#define EXT_FRAMEBUFFER_LEN TFT_FRAMEBUFFER_LEN

extern volatile uint16_t TFT_FrameBuffer[EXT_FRAMEBUFFER_LEN];
void ExtSram_Init(void);

#endif /* __EXT_SRAM_H__ */
