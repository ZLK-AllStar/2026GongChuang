#include "ext_sram.h"

/* One 320x120 RGB565 tile fits comfortably in the STM32F407 internal SRAM. */
volatile uint16_t TFT_FrameBuffer[EXT_FRAMEBUFFER_LEN]
  __attribute__((aligned(4)));

void ExtSram_Init(void)
{
  /* Retain the old entry point for source compatibility.  The framebuffer is
   * statically allocated in internal SRAM, so no peripheral setup is needed. */
}


