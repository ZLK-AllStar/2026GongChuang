/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hmi_link.h
  * @brief   Uplink to the external ApusIDE HMI panel over USART3.
  *
  *          The panel (ApusIDE project "TEST") receives a single-byte command
  *          code and renders a geometry/colour asset.  This module owns that
  *          transport and exposes it as a small API so application code never
  *          touches the UART directly.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __HMI_LINK_H__
#define __HMI_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include "hmi_codes.h"

/* USART3 link: PB10 (TX) -> panel RX, PB11 (RX) <- panel TX, 115200 8N1. */
#define HMI_LINK_TX_TIMEOUT_MS   100U


/* Panel codes: 1..72 retain the 8 x 9 table; 73..80 are white by shape; 81..90 are cone colors. */
#define HMI_LINK_SHAPE_COUNT     HMI_SHAPE_COUNT
#define HMI_LINK_COLOR_COUNT     HMI_COLOR_COUNT
#define HMI_LINK_CODE_MIN        HMI_CODE_MIN
#define HMI_LINK_CODE_MAX        HMI_CODE_MAX

/* Shape index — panel order:
 * 正四面体/三棱柱/五棱柱/六棱柱/圆柱形/长方体/正方体/球体/圆锥. */
typedef enum
{
  HMI_SHAPE_TETRAHEDRON = 0,  /* 正四面体 */
  HMI_SHAPE_TRI_PRISM,        /* 三棱柱   */
  HMI_SHAPE_PENTA_PRISM,      /* 五棱柱   */
  HMI_SHAPE_HEXA_PRISM,       /* 六棱柱   */
  HMI_SHAPE_CYLINDER,         /* 圆柱形   */
  HMI_SHAPE_CUBOID,           /* 长方体   */
  HMI_SHAPE_CUBE,             /* 正方体   */
  HMI_SHAPE_SPHERE,           /* 球体     */
  HMI_SHAPE_CONE              /* 圆锥: codes 81..90 */
} HMI_Shape_t;

/* Colour index — panel order: 红/橙/黄/绿/青/蓝/紫/灰/黑/白. */
typedef enum
{
  HMI_COLOR_RED = 0,          /* 红 */
  HMI_COLOR_ORANGE,           /* 橙 */
  HMI_COLOR_YELLOW,           /* 黄 */
  HMI_COLOR_GREEN,            /* 绿 */
  HMI_COLOR_CYAN,             /* 青 */
  HMI_COLOR_BLUE,             /* 蓝 */
  HMI_COLOR_PURPLE,           /* 紫 */
  HMI_COLOR_GRAY,             /* 灰 */
  HMI_COLOR_BLACK,            /* 黑 */
  HMI_COLOR_WHITE             /* 白: codes 73..80 for original shapes, 90 for cone */
} HMI_Color_t;

/* Bring up the USART3 transport.  Call once after MX_USART3_UART_Init(). */
void HMI_LinkInit(void);
bool HMI_SendSessionReset(void);
bool HMI_SendRecord(uint8_t code, uint8_t id);
uint32_t HMI_GetErrorCount(void);

/* Pack shape/colour into a panel command code (1..90; invalid pair returns 0). */
uint8_t HMI_MakeCode(HMI_Shape_t shape, HMI_Color_t color);

/* Inverse of HMI_MakeCode: the shape index carried by a panel code.  A code
 * outside 1..90 reports HMI_SHAPE_TETRAHEDRON. */
HMI_Shape_t HMI_CodeShape(uint8_t code);

/* Send a raw command code; returns true on local UART TX completion (no panel ACK). */
bool HMI_SendCode(uint8_t code);

/* Send the code for one geometry/colour pair. */
bool HMI_SendShapeColor(HMI_Shape_t shape, HMI_Color_t color);

#ifdef __cplusplus
}
#endif

#endif /* __HMI_LINK_H__ */
