/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    host_link.h
  * @brief   Downlink to the vision host (Jetson running _cv2.py) over USART2.
  *
  *          上位机协议（与 _cv2.py 保持一致）：
  *          Jetson -> STM32 抓取帧  AA 55 01 SEQ CMD X_L X_H Y_L Y_H CHECK（10 字节）
  *          Jetson -> STM32 启动通知帧 AA 55 02 SEQ READY CHECK（6 字节）
  *          STM32  -> Jetson 状态帧 55 AA 81 SEQ STATUS CHECK（6 字节）
  *          STM32  -> Jetson 按键帧 55 AA 82 SEQ STATE CHECK（6 字节）
  *          CHECK 均为前面所有字节的和低 8 位；X/Y 为小端 uint16 毫米值；
  *          CMD 即 HMI 屏幕的 1..90 形状×颜色指令码，可直接转发给屏幕。
  *
  *          按键帧上报 PE2 实体按键的电平边沿：按下 0x01、松开 0x00。
  *          上位机以此进入/退出识别流程，不需要额外的握手字节。
  *          启动通知帧只用于告知“上位机已启动完成”，让本地屏幕从等待上位机
  *          切换到等待按键；它不强制与按键的先后顺序，也不放行任何动作。
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __HOST_LINK_H__
#define __HOST_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* USART2 link: PA2 (TX) -> Jetson RX, PA3 (RX) <- Jetson TX, 115200 8N1. */
#define HOST_LINK_TX_TIMEOUT_MS     100U

/* 等待抓取帧期间轮询本地按键的间隔；按键消抖本身在 button.c 中完成。 */
#define HOST_LINK_POLL_INTERVAL_MS  5U

/* 上位机抓取帧：帧头 + 类型 + 序号 + 指令码 + 坐标 + 校验和。 */
#define HOST_LINK_HEADER_0          0xAAU
#define HOST_LINK_HEADER_1          0x55U
#define HOST_LINK_FRAME_LEN         10U
#define HOST_LINK_PICK_TYPE         0x01U

/* 上位机启动通知帧：与下行帧同头，仅 TYPE 不同，READY 固定 0x01。
 * 上位机用它告知“已启动”；下位机在收到它之前也用同类型回告“本机在线”。 */
#define HOST_LINK_HELLO_TYPE        0x02U
#define HOST_LINK_HELLO_FRAME_LEN   6U
#define HOST_LINK_HELLO_READY       0x01U
#define HOST_LINK_HELLO_SEQ         0U

/* 下位机状态帧帧头与类型。 */
#define HOST_LINK_STATUS_HDR_0      0x55U
#define HOST_LINK_STATUS_HDR_1      0xAAU
#define HOST_LINK_STATUS_TYPE       0x81U
#define HOST_LINK_STATUS_FRAME_LEN  6U

/* 下位机按键事件帧：与状态帧同长同头，仅 TYPE 不同。 */
#define HOST_LINK_EVENT_TYPE          0x82U
#define HOST_LINK_EVENT_FRAME_LEN     6U
#define HOST_LINK_EVENT_STATE_RELEASE 0x00U
#define HOST_LINK_EVENT_STATE_PRESS   0x01U

/* 状态码取值与 _cv2.py 的 STATUS_* 常量完全一致。 */
typedef enum
{
  HOST_STATUS_ACK          = 0x01,  /* 已接受任务 */
  HOST_STATUS_BUSY         = 0x02,  /* 正在执行 */
  HOST_STATUS_DONE         = 0x03,  /* 机械动作完成 */
  HOST_STATUS_FAILED       = 0x04,  /* 执行失败 */
  HOST_STATUS_OUT_OF_RANGE = 0x05   /* 坐标越界 */
} Host_Status_t;

/* 一帧已通过帧头/类型/校验和检查的抓取指令。 */
typedef struct
{
  uint8_t sequence;   /* 任务序号 0..255 */
  uint8_t command;    /* 1..90 形状×颜色指令码 */
  uint16_t x_raw;     /* 帧内原始 X 毫米值 */
  uint16_t y_raw;     /* 帧内原始 Y 毫米值 */
  float x_mm;         /* X 轴机械坐标（毫米） */
  float y_mm;         /* Y 轴机械坐标（毫米） */
} Host_PickFrame_t;

typedef void (*Host_FrameHandler_t)(const Host_PickFrame_t *frame);
void HostLink_SetFrameHandler(Host_FrameHandler_t handler);
void HostLink_Service(void);
uint32_t HostLink_GetOverflowCount(void);
uint32_t HostLink_GetErrorCount(void);

/* 等待抓取帧期间周期性调用的钩子（例如轮询本地按键）。 */
typedef void (*Host_PollHook_t)(void);

/* 等待时长哨兵：一直等到有帧为止。 */
#define HOST_LINK_WAIT_FOREVER      0xFFFFFFFFU

/* 启动 USART2 接收（含 NVIC 使能）。在 MX_USART2_UART_Init() 之后调用一次。 */
void HostLink_Init(void);

/* 注册等待期间的轮询钩子；不需要时传 NULL 清除。 */
void HostLink_SetPollHook(Host_PollHook_t hook);

/* 阻塞等待一帧协议合法的抓取指令。超时返回 false；WAIT_FOREVER 时不会超时。
 * 只做协议校验（帧头/类型/校验和），指令码与坐标范围由调用方判定。
 * 等待期间按 HOST_LINK_POLL_INTERVAL_MS 的节奏调用已注册的轮询钩子。 */
bool HostLink_GetPickFrame(Host_PickFrame_t *frame, uint32_t timeout_ms);

/* 向上位机发送一帧状态（阻塞式）。 */
bool HostLink_SendStatus(uint8_t sequence, Host_Status_t status);

/* 向上位机发送一帧按键事件（阻塞式）。state 取 EVENT_STATE_PRESS/RELEASE。 */
bool HostLink_SendKeyEvent(uint8_t sequence, uint8_t state);

/* 向上位机发送一帧“本机在线”告知（阻塞式）。下位机在还没收到上位机帧之前
 * 按周期重发，让上位机日志能显示“下位机在线”；只影响界面与日志，不影响
 * 任何动作许可。 */
bool HostLink_SendHostReady(void);

/* Number of local presses reported by HostLink_SendKeyEvent() since the last
 * call. Reading clears the count. Serial input never increments this count. */
uint8_t HostLink_GetKeyPress(void);

/* 是否已经收到上位机的启动通知帧。仅用于界面提示“上位机已就绪”，
 * 不参与任何动作许可判定。 */
bool HostLink_GetHostReady(void);

#ifdef __cplusplus
}
#endif

#endif /* __HOST_LINK_H__ */
