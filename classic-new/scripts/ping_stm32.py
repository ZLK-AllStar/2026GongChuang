#!/usr/bin/env python3
"""树莓派/Jetson 侧 UART 自检：环回 + 与下位机握手。

无需改接线即可先判 Jetson 的 TX 是否正常：
  python3 scripts/ping_stm32.py --loopback
    → 需要把 40-pin 的 Pin 8 (TX) 和 Pin 10 (RX) 用杜邦线短接后再运行。
    → 能读回自己发的字节 = Jetson 侧 UART 收发正常，问题在外部线缆或下位机。
    → 读不回 = Jetson 的 TX 路径/权限/端口本身有问题。

不带参数时做下位机握手（需要先停掉 huatai-vision 服务）：
  python3 scripts/ping_stm32.py
  心跳仅能证明板端发送正常；未回包也可能是固件解析错误或仍在等待实体按键。
"""

import sys
import time

try:
    import serial
except ImportError:
    print("pyserial is required: pip3 install pyserial", file=sys.stderr)
    raise SystemExit(2)

PORT = "/dev/ttyTHS1"
BAUDRATE = 115200

STATUS_TYPE = 0x81
KEY_TYPE = 0x82
HELLO_TYPE = 0x02
STATUS_NAMES = {0x01: "ACK", 0x02: "BUSY", 0x03: "DONE", 0x04: "FAILED", 0x05: "OUT_OF_RANGE"}


def build(type_byte: int, sequence: int, value: int, header=(0xAA, 0x55)) -> bytes:
    body = bytes((header[0], header[1], type_byte, sequence, value))
    return body + bytes((sum(body) & 0xFF,))


def collect(link: "serial.Serial", seconds: float) -> list:
    """读若干秒，返回 [(type, seq, value)]。"""

    deadline = time.time() + seconds
    buffer = bytearray()
    found = []
    while time.time() < deadline:
        waiting = link.in_waiting
        if waiting:
            buffer.extend(link.read(waiting))
            while len(buffer) >= 6:
                if buffer[0] != 0x55 or buffer[1] != 0xAA:
                    del buffer[0]
                    continue
                if (sum(buffer[:5]) & 0xFF) != buffer[5]:
                    del buffer[0]
                    continue
                found.append((buffer[2], buffer[3], buffer[4]))
                del buffer[:6]
        else:
            time.sleep(0.02)
    return found


def describe(frames: list) -> str:
    parts = []
    for type_byte, seq, value in frames:
        if type_byte == HELLO_TYPE:
            parts.append("hello(online)")
        elif type_byte == STATUS_TYPE:
            parts.append(f"status seq={seq} {STATUS_NAMES.get(value, hex(value))}")
        elif type_byte == KEY_TYPE:
            parts.append(f"key seq={seq} state={value}")
        else:
            parts.append(f"type=0x{type_byte:02x}")
    return ", ".join(parts) if parts else "(nothing)"


def loopback_test() -> int:
    """需要 Pin 8 与 Pin 10 短接；验证 Jetson 自身收发。"""

    marker = bytes((0xA5, 0x5A, 0x01, 0x02, 0x03, 0x04))
    link = serial.Serial(PORT, BAUDRATE, timeout=1.0, write_timeout=0.2)
    try:
        link.reset_input_buffer()
        link.write(marker)
        link.flush()
        echoed = link.read(len(marker))
        print(f"sent:     {marker.hex(' ')}")
        print(f"received: {echoed.hex(' ') if echoed else '(nothing)'}")
        if echoed == marker:
            print("VERDICT: Jetson UART TX and RX both work.")
            print("         -> the fault is in the cable to the STM32, or on the STM32 side.")
            return 0
        if not echoed:
            print("VERDICT: nothing came back.")
            print("         -> if Pin 8 and Pin 10 really are shorted, the Jetson TX path is broken.")
            return 1
        print("VERDICT: received different bytes than sent; check baud rate and wiring.")
        return 1
    finally:
        link.close()


def board_test() -> int:
    link = serial.Serial(PORT, BAUDRATE, timeout=0, write_timeout=0.2)
    try:
        print("step 1: listening 3 s before sending anything...")
        before = collect(link, 3.0)
        print(f"  received: {describe(before)}")
        print("  (hello frames confirm the STM32 -> Jetson receive path)")

        print("step 2: sending host startup notification (AA 55 02 00 01 02)...")
        link.write(build(HELLO_TYPE, 0, 0x01, header=(0xAA, 0x55)))
        link.flush()
        after_hello = collect(link, 2.5)
        print(f"  received: {describe(after_hello)}")
        print("  (heartbeats stopping means the board parsed our frame)")

        print("step 3: sending an out-of-range pick frame (x=600mm)...")
        # x=600 exceeds travel. An armed board returns 0x05 without moving;
        # a board waiting for its physical button intentionally drops the frame.
        pick_oot = bytes((0xAA, 0x55, 0x01, 0x11, 0x01, 0x58, 0x02, 0x00, 0x00))
        pick_oot += bytes((sum(pick_oot) & 0xFF,))
        print(f"  frame: {pick_oot.hex(' ')}")
        link.write(pick_oot)
        link.flush()
        after_pick = collect(link, 3.0)
        print(f"  received: {describe(after_pick)}")

        heartbeats_after_hello = sum(1 for t, _s, _v in after_hello if t == HELLO_TYPE)
        heartbeats_after_pick = sum(1 for t, _s, _v in after_pick if t == HELLO_TYPE)
        got_oor = any(t == STATUS_TYPE and v == 0x05 for t, _s, v in after_pick)

        print()
        if got_oor:
            print("VERDICT: board received the pick frame and answered 0x05.")
            print("         -> the pick path works; the earlier ACK timeout had another cause.")
            return 0
        if heartbeats_after_hello and heartbeats_after_pick:
            print("VERDICT: the board keeps sending online heartbeats after both frames,")
            print("         but this does not prove a wiring fault.")
            print("         -> check the 6-byte hello parser and local button arming.")
            print("            A displayed CMD proves the pick frame was parsed, even")
            print("            when WAIT_BUTTON then drops it without sending ACK.")
            return 1
        print("VERDICT: no matching status; the result is inconclusive.")
        print("         -> check WAIT_BUTTON vs BUSY, the parser and serial ownership.")
        return 1
    finally:
        link.close()


def main() -> int:
    if "--loopback" in sys.argv:
        return loopback_test()
    return board_test()


if __name__ == "__main__":
    sys.exit(main())
