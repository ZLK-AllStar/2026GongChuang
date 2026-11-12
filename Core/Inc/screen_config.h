/* Application-owned display hardware options.  CubeMX is intentionally not
 * involved in this file.  Change these values for a different board wiring. */
#ifndef __SCREEN_CONFIG_H__
#define __SCREEN_CONFIG_H__

/* The display no longer depends on the FSMC/external-SRAM bus. */
#define SCREEN_FSMC_ENABLE             0U
#define SCREEN_FSMC_USE_A4             0U

/* ST7789 BLK is active high on the current board. */
#define SCREEN_BLK_DEFAULT_ON          1U

/* Only the diagnostic task page is active in this architecture. */
#define SCREEN_PAGE_SELECT              0U

/* USART3 external HMI handles classification; ST7789 is diagnostics only. */
#define SCREEN_LEGACY_HMI_ENABLE       1U

#if SCREEN_PAGE_SELECT != 0U
#error "SCREEN_PAGE_SELECT must be 0 (diagnostics)"
#endif

#endif /* __SCREEN_CONFIG_H__ */
