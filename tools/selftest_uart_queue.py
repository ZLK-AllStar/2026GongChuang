"""Test the actual USART1 receive queue with delayed foreground services.

Only the queue's CubeMX USER CODE regions are compiled; generated GPIO/UART
setup is excluded so the test requires GCC, but no STM32 hardware.
"""
import re
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def user_region(source, tag):
    begin = f"/* USER CODE BEGIN {tag} */"
    end = f"/* USER CODE END {tag} */"
    return source.split(begin, 1)[1].split(end, 1)[0]


def main():
    source = (ROOT / "Core/Src/usart.c").read_text(encoding="utf-8")
    header = (ROOT / "Core/Inc/usart.h").read_text(encoding="utf-8")
    defines = "\n".join(
        re.findall(r"^\s*#define\s+(?:CMD_LEN|UART_RX_QUEUE_DEPTH)\s+\d+.*$", header, re.M)
    )
    globals_text = re.sub(r"^#include.*$", "", user_region(source, "0"), flags=re.M)
    harness = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define __IO volatile
#define __disable_irq() ((void)0)
#define __enable_irq() ((void)0)
uint32_t HAL_GetTick(void);
void AppRuntime_Service(void);
void UART_RxPushFrameFromIsr(uint16_t count);
'''
    harness += defines + "\n" + globals_text + user_region(source, "1")
    harness += r'''
static uint32_t ticks, service_duration;
static bool inject_reply;
static unsigned calls;

uint32_t HAL_GetTick(void) { return ticks; }

static void Push(uint8_t address)
{
  const uint8_t response[] = {address, 0x36, 0, 0, 0, 0, 0, 0x6B};
  memcpy((void *)rxDmaBuffer, response, sizeof(response));
  UART_RxPushFrameFromIsr(sizeof(response));
}

void AppRuntime_Service(void)
{
  calls++;
  if (inject_reply) {
    /* ISR receives a valid position response before a 50 ms read deadline,
     * while the foreground is occupied rendering/flushing the display. */
    ticks += 5U;
    Push(3);
    ticks += service_duration - 5U;
    inject_reply = false;
  } else {
    ticks += service_duration;
  }
}

static void Reset(uint32_t start, uint32_t duration, bool reply)
{
  UART_RxQueueReset();
  ticks = start;
  service_duration = duration;
  inject_reply = reply;
  calls = 0;
}

int main(void)
{
  unsigned before;
  Reset(0U, 65U, true);
  assert(UART_RxPopFrame(50U));
  assert(calls == 1U && ticks == 65U && rxQueueCount == 0U);
  assert(rxCount == 8U && rxCmd[0] == 3U && rxCmd[1] == 0x36U);
  puts("PASS queued feedback survives foreground service exceeding read timeout");

  Reset(UINT32_MAX - 20U, 65U, true);
  assert(UART_RxPopFrame(50U) && rxCmd[0] == 3U);
  puts("PASS queued feedback survives tick wrap during foreground service");

  Reset(0U, 1U, false);
  assert(!UART_RxPopFrame(50U) && ticks == 50U && rxQueueCount == 0U);
  Reset(UINT32_MAX - 20U, 1U, false);
  assert(!UART_RxPopFrame(50U) && calls == 50U);
  puts("PASS missing feedback still times out, including tick wrap");

  Reset(0U, 65U, false);
  assert(!UART_RxPopFrame(50U) && calls == 1U);
  puts("PASS slow service without feedback remains a timeout");

  Reset(0U, 1U, false);
  Push(1);
  Push(2);
  Push(3);
  before = calls;
  assert(UART_RxPopFrame(0U) && rxCmd[0] == 1U && rxFrameFlag);
  assert(UART_RxPopFrame(0U) && rxCmd[0] == 2U && rxFrameFlag);
  assert(UART_RxPopFrame(0U) && rxCmd[0] == 3U && !rxFrameFlag);
  assert(calls == before);
  assert(!UART_RxPopFrame(0U));
  puts("PASS buffered frames preserve FIFO order and zero-wait polling");
  return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="huatai-uart-queue-") as folder:
        work = Path(folder)
        test_source = work / "test_uart_queue.c"
        test_source.write_text(harness, encoding="utf-8")
        executable = work / "test_uart_queue.exe"
        subprocess.run([
            "gcc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            str(test_source), "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
