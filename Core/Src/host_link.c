/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    host_link.c
  * @brief   Vision host (Jetson / _cv2.py) link over USART2: pick-frame
  *          reception, key-event reporting and status-frame transmission.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "host_link.h"
#include "usart.h"
#include "app_config.h"
#include "app_display.h"
#include <string.h>

/* 接收环形缓冲：单生产者（USART2 中断）单消费者（主循环），索引访问原子。 */
#define HOST_LINK_RING_SIZE       APP_HOST_RING_SIZE
static bool host_initialized;
static bool host_service_active;
static Host_FrameHandler_t frame_handler;
static volatile uint32_t host_overflows, host_uart_errors;
static uint32_t host_tx_errors;
static volatile bool host_ring_damaged;

/* 解析暂存区：可容纳 4 个完整抓取帧，主循环独占访问。 */
#define HOST_LINK_PARSE_SIZE      (HOST_LINK_FRAME_LEN * 4U)

static volatile uint8_t host_ring[HOST_LINK_RING_SIZE];
static volatile uint16_t host_ring_head = 0U;  /* ISR 写指针 */
static volatile uint16_t host_ring_tail = 0U;  /* 主循环读指针 */

static uint8_t host_parse_buf[HOST_LINK_PARSE_SIZE];
static uint16_t host_parse_len = 0U;

/* 单字节接收缓冲：中断每收满 1 字节回调一次。 */
static uint8_t host_rx_byte;

/* Local button presses are recorded when reported and consumed by the main loop.
 * Receiving a serial frame must never grant permission to move. */
static Host_PollHook_t host_poll_hook = NULL;
static volatile uint8_t host_key_presses = 0U;

/* 上位机启动通知标志：只影响界面提示，不放行任何动作。 */
static bool host_ready = false;

/* 最近一次收到任何上位机帧的时间戳，用于判断上位机是否还在线。 */
static uint32_t host_last_frame_ms = 0U;

/* 收到最后一帧后多久算上位机离线；上位机在线时至少每秒有一帧。 */
#define HOST_LINK_READY_TIMEOUT_MS  5000U

static void HostLink_PushByteFromIsr(uint8_t byte);
static void HostLink_DrainRing(void);
static bool HostLink_TryParseFrame(Host_PickFrame_t *frame);
static void HostLink_ConsumeParsed(uint16_t count);
static bool HostLink_SendFrame(uint8_t type, uint8_t sequence, uint8_t value);

void HostLink_Init(void)
{
  /* USART2 中断未在 CubeMX 中使能，这里在应用层补上（优先级低于电机总线）。 */
  HAL_NVIC_SetPriority(USART2_IRQn, 5U, 0U);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  /* 恢复可能残留错误状态的 UART，复位缓冲后再启动接收。 */
  __HAL_UART_CLEAR_OREFLAG(&huart2);
  __HAL_UART_CLEAR_FEFLAG(&huart2);
  __HAL_UART_CLEAR_NEFLAG(&huart2);
  __HAL_UART_CLEAR_PEFLAG(&huart2);
  huart2.ErrorCode = HAL_UART_ERROR_NONE;

  host_initialized = true;
  host_ring_damaged = false;
  host_ring_head = 0U;
  host_ring_tail = 0U;
  host_parse_len = 0U;
  host_key_presses = 0U;
  host_ready = false;

  (void)HAL_UART_Receive_IT(&huart2, &host_rx_byte, 1U);
}

void HostLink_SetPollHook(Host_PollHook_t hook)
{
  host_poll_hook = hook;
}

uint8_t HostLink_GetKeyPress(void)
{
  uint8_t presses = host_key_presses;

  host_key_presses = 0U;
  return presses;
}

bool HostLink_GetHostReady(void)
{
  /* 上位机停止发送（进程退出、拔线、板子重启）后，通知标志不能一直为真，
   * 否则屏幕会永远显示“已就绪”。上位机在线时至少每秒有一帧（心跳或抓取帧
   * 期间的密集帧），因此超过 HOST_LINK_READY_TIMEOUT_MS 没有收到任何帧就
   * 视为离线；下一帧到达会自动恢复。 */
  if (host_ready &&
      (uint32_t)(HAL_GetTick() - host_last_frame_ms) >= HOST_LINK_READY_TIMEOUT_MS)
  {
    return false;
  }
  return host_ready;
}

bool HostLink_GetPickFrame(Host_PickFrame_t *frame, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  uint32_t last_poll = start;

  for (;;)
  {
    uint32_t now;

    HostLink_DrainRing();
    if (HostLink_TryParseFrame(frame))
    {
      return true;
    }

    now = HAL_GetTick();
    if (host_poll_hook != NULL &&
        (uint32_t)(now - last_poll) >= HOST_LINK_POLL_INTERVAL_MS)
    {
      last_poll = now;
      host_poll_hook();
    }

    if (timeout_ms != HOST_LINK_WAIT_FOREVER &&
        (uint32_t)(now - start) >= timeout_ms)
    {
      return false;
    }
    HAL_Delay(1U);
  }
}

bool HostLink_SendStatus(uint8_t sequence, Host_Status_t status)
{
  return HostLink_SendFrame(HOST_LINK_STATUS_TYPE, sequence, (uint8_t)status);
}

bool HostLink_SendKeyEvent(uint8_t sequence, uint8_t state)
{
  if (state == HOST_LINK_EVENT_STATE_PRESS && host_key_presses < UINT8_MAX)
  {
    host_key_presses++;
  }
  return HostLink_SendFrame(HOST_LINK_EVENT_TYPE, sequence, state);
}

bool HostLink_SendHostReady(void)
{
  if (host_ready)
  {
    /* 已经收到上位机的启动通知：不需要再告知，避免占用串口。 */
    return true;
  }
  return HostLink_SendFrame(HOST_LINK_HELLO_TYPE, HOST_LINK_HELLO_SEQ, HOST_LINK_HELLO_READY);
}

/* 6 字节上报帧共用组帧与发送逻辑：状态帧和按键事件帧只差 TYPE 与状态字节。 */
static bool HostLink_SendFrame(uint8_t type, uint8_t sequence, uint8_t value)
{
  uint8_t packet[HOST_LINK_STATUS_FRAME_LEN];
  uint8_t sum = 0U;
  uint8_t i;

  packet[0] = HOST_LINK_STATUS_HDR_0;
  packet[1] = HOST_LINK_STATUS_HDR_1;
  packet[2] = type;
  packet[3] = sequence;
  packet[4] = value;
  for (i = 0U; i < 5U; i++)
  {
    sum = (uint8_t)(sum + packet[i]);
  }
  packet[5] = sum;

  if (HAL_UART_Transmit(&huart2, packet, HOST_LINK_STATUS_FRAME_LEN,
                        HOST_LINK_TX_TIMEOUT_MS) != HAL_OK) { host_tx_errors++; return false; }
  return true;
}

/* 把环形缓冲中的数据搬入解析暂存区。数据先读、索引后动，
 * 与中断侧写入顺序一致，无需关中断。 */
static void HostLink_DrainRing(void)
{
  if (host_ring_damaged) {
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();
    host_ring_tail = host_ring_head;
    host_parse_len = 0U;
    host_ring_damaged = false;
    __set_PRIMASK(irq_state);
  }
  while (host_parse_len < HOST_LINK_PARSE_SIZE &&
         host_ring_tail != host_ring_head)
  {
    host_parse_buf[host_parse_len] = host_ring[host_ring_tail];
    host_parse_len++;
    host_ring_tail = (uint16_t)((host_ring_tail + 1U) % HOST_LINK_RING_SIZE);
  }
}

/* 在解析暂存区中扫描帧头并尝试解出一帧。
 * 两种接收帧都是固定长度的和校验帧，帧头首字节不同，因此可以在同一个
 * 流里交叉同步：
 *   抓取帧  AA 55 01 SEQ CMD X_L X_H Y_L Y_H CHECK（10 字节）
 *   按键帧  55 AA 82 SEQ STATE CHECK（6 字节）
 * 帧头前的杂字节直接丢弃；校验失败只丢 1 个字节，重新同步。
 * 数据不足时保留可能的帧头字节，等待后续数据。 */
static bool HostLink_TryParseFrame(Host_PickFrame_t *frame)
{
  while (host_parse_len > 0U)
  {
    uint16_t i = 0U;



    while (i < host_parse_len && host_parse_buf[i] != HOST_LINK_HEADER_0 &&
           host_parse_buf[i] != HOST_LINK_STATUS_HDR_0)
    {
      i++;
    }

    if (i > 0U)
    {
      HostLink_ConsumeParsed(i);
    }

    if (host_parse_len < 2U)
    {
      /* 只剩 1 个字节：可能是 0xAA 或 0x55，保留等待下一字节。 */
      return false;
    }

    if (host_parse_buf[0] == HOST_LINK_HEADER_0 &&
        host_parse_buf[1] == HOST_LINK_HEADER_1)
    {
      uint8_t frame_len;
      uint8_t sum = 0U;
      uint8_t k;

      if (host_parse_len < 3U)
      {
        return false;
      }

      /* The pick and startup frames share a header but have different lengths.
       * Select the length before waiting for the rest of the frame; otherwise a
       * 6-byte startup frame is incorrectly held until ten bytes arrive. */
      if (host_parse_buf[2] == HOST_LINK_PICK_TYPE)
      {
        frame_len = HOST_LINK_FRAME_LEN;
      }
      else if (host_parse_buf[2] == HOST_LINK_HELLO_TYPE)
      {
        frame_len = HOST_LINK_HELLO_FRAME_LEN;
      }
      else
      {
        HostLink_ConsumeParsed(1U);
        continue;
      }

      if (host_parse_len < frame_len)
      {
        return false;
      }

      for (k = 0U; k < (uint8_t)(frame_len - 1U); k++)
      {
        sum = (uint8_t)(sum + host_parse_buf[k]);
      }

      if (host_parse_buf[2] == HOST_LINK_PICK_TYPE &&
          sum == host_parse_buf[frame_len - 1U])
      {
        host_last_frame_ms = HAL_GetTick();
        frame->sequence = host_parse_buf[3];
        frame->command = host_parse_buf[4];
        frame->x_raw = (uint16_t)host_parse_buf[5] |
                       (uint16_t)((uint16_t)host_parse_buf[6] << 8);
        frame->y_raw = (uint16_t)host_parse_buf[7] |
                       (uint16_t)((uint16_t)host_parse_buf[8] << 8);
        frame->x_mm = (float)frame->x_raw;
        frame->y_mm = (float)frame->y_raw;
        AppDisplay_SetHostFrame(frame->sequence, frame->command,
                                frame->x_raw, frame->y_raw);
        HostLink_ConsumeParsed(HOST_LINK_FRAME_LEN);
        return true;
      }

      /* 上位机启动通知：置位界面提示标志，不作为抓取指令返回。 */
      if (host_parse_buf[2] == HOST_LINK_HELLO_TYPE &&
          host_parse_buf[4] == HOST_LINK_HELLO_READY &&
          sum == host_parse_buf[frame_len - 1U])
      {
        host_last_frame_ms = HAL_GetTick();
        host_ready = true;
        HostLink_ConsumeParsed(HOST_LINK_HELLO_FRAME_LEN);
        continue;
      }
    }
    else if (host_parse_buf[1] == HOST_LINK_STATUS_HDR_1)
    {
      /* 上位机不会向 STM32 发事件帧；若真的收到，确认校验后直接吞掉。 */
      uint8_t sum = 0U;
      uint8_t k;

      if (host_parse_len < HOST_LINK_EVENT_FRAME_LEN)
      {
        return false;
      }

      for (k = 0U; k < (uint8_t)(HOST_LINK_EVENT_FRAME_LEN - 1U); k++)
      {
        sum = (uint8_t)(sum + host_parse_buf[k]);
      }

      if (host_parse_buf[2] == HOST_LINK_EVENT_TYPE &&
          sum == host_parse_buf[HOST_LINK_EVENT_FRAME_LEN - 1U])
      {
        HostLink_ConsumeParsed(HOST_LINK_EVENT_FRAME_LEN);
        continue;
      }
    }

    /* 不是本工程的接收帧：丢弃帧头首字节，继续向后同步。 */
    HostLink_ConsumeParsed(1U);
  }

  return false;
}

static void HostLink_ConsumeParsed(uint16_t count)
{
  if (count >= host_parse_len)
  {
    host_parse_len = 0U;
    return;
  }
  memmove(host_parse_buf, &host_parse_buf[count],
          (size_t)(host_parse_len - count));
  host_parse_len = (uint16_t)(host_parse_len - count);
}

static void HostLink_PushByteFromIsr(uint8_t byte)
{
  uint16_t next_head = (uint16_t)((host_ring_head + 1U) % HOST_LINK_RING_SIZE);

  if (next_head == host_ring_tail)
  {
    host_overflows++;
    host_ring_damaged = true;
    return;
  }

  host_ring[host_ring_head] = byte;
  host_ring_head = next_head;
}

/* USER CODE BEGIN HAL_CALLBACKS */
/* 全工程唯一的 HAL UART 接收完成回调：只接管 USART2，
 * USART1（电机总线，DMA 接收）的事件一律忽略。 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (host_initialized && huart->Instance == USART2)
  {
    HostLink_PushByteFromIsr(host_rx_byte);
    (void)HAL_UART_Receive_IT(&huart2, &host_rx_byte, 1U);
  }
}

/* 溢出/帧错误等会终止中断接收；清标志后重新武装接收。 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (host_initialized && huart->Instance == USART2)
  {
    host_uart_errors++;
    host_ring_damaged = true;
    /* HAL leaves RxState BUSY_RX for some recoverable errors.  In that
     * state Receive_IT() returns HAL_BUSY and the link silently stops
     * receiving forever.  Abort first so the handle is returned to READY. */
    (void)HAL_UART_AbortReceive(&huart2);
    __HAL_UART_CLEAR_OREFLAG(&huart2);
    __HAL_UART_CLEAR_FEFLAG(&huart2);
    __HAL_UART_CLEAR_NEFLAG(&huart2);
    __HAL_UART_CLEAR_PEFLAG(&huart2);
    huart2.ErrorCode = HAL_UART_ERROR_NONE;
    (void)HAL_UART_Receive_IT(&huart2, &host_rx_byte, 1U);
  }
}
/* USER CODE END HAL_CALLBACKS */

void HostLink_SetFrameHandler(Host_FrameHandler_t handler) { frame_handler = handler; }
void HostLink_Service(void)
{
  Host_PickFrame_t frame;
  unsigned count;
  if (!host_initialized || host_service_active) { return; }
  host_service_active = true;
  /* Bounded work per service call, so continuous noise cannot starve motion. */
  for (count = 0U; count < 4U; count++) {
    HostLink_DrainRing();
    if (!HostLink_TryParseFrame(&frame)) { break; }
    if (frame_handler) { frame_handler(&frame); }
  }
  host_service_active = false;
}
uint32_t HostLink_GetOverflowCount(void) { return host_overflows; }
uint32_t HostLink_GetErrorCount(void) { return host_uart_errors + host_tx_errors; }
