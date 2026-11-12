#!/usr/bin/env python3
"""离线验证 _cv2.py 的按键帧解析与 WAIT_BUTTON 状态机。

不依赖串口硬件和实体按键：导入 _cv2.py 后用对象替身替换 SerialLink 的连接，
注入 STM32 侧会发送的字节（按键事件帧和状态帧），再驱动状态机并检查状态。

用法（Jetson 或开发机，虚拟环境内）：
    python3 scripts/selftest_button_logic.py
"""

import importlib.util
import re
import sys
import time
from pathlib import Path

SCRIPT = Path(__file__).resolve().parent.parent / "_cv2.py"


def load_module():
    spec = importlib.util.spec_from_file_location("_cv2_under_test", SCRIPT)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load {SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class FakeConnection:
    """扮演 pyserial 的 Serial 对象：可控注入字节、记录写出的抓取帧。"""

    def __init__(self) -> None:
        self.pending = bytearray()
        self.written = bytearray()
        self.closed = False

    def inject(self, data: bytes) -> None:
        self.pending.extend(data)

    @property
    def in_waiting(self) -> int:
        return len(self.pending)

    def read(self, count: int) -> bytes:
        chunk = bytes(self.pending[:count])
        del self.pending[:count]
        return chunk

    def write(self, data: bytes) -> int:
        self.written.extend(data)
        return len(data)

    def close(self) -> None:
        self.closed = True


def build_key_frame(sequence: int, state: int) -> bytes:
    body = bytes((0x55, 0xAA, 0x82, sequence, state))
    return body + bytes((sum(body) & 0xFF,))


def main() -> int:
    module = load_module()
    checks = []

    # ---- 1. 帧解析 ----
    # 用 __new__ 绕开 __init__（那会真的去打开串口），手动补齐实例状态。
    link = module.SerialLink.__new__(module.SerialLink)
    link.debug = False
    link.receive_buffer = bytearray()
    link.key_presses = 0
    link.key_pressed = False
    link.board_ready = False
    connection = FakeConnection()
    link.connection = connection

    connection.inject(build_key_frame(7, module.KEY_STATE_PRESS))
    messages = link.poll_status()
    checks.append(("key press frame parsed", link.take_key_presses() == 1))
    checks.append(("key frame yields no status message", messages == []))

    connection.inject(build_key_frame(8, module.KEY_STATE_RELEASE))
    link.poll_status()
    checks.append(("key release frame ignored", link.take_key_presses() == 0))
    checks.append(("release clears pressed flag", link.key_pressed is False))

    status_body = bytes((0x55, 0xAA, 0x81, 7, module.STATUS_ACK))
    connection.inject(status_body + bytes((sum(status_body) & 0xFF,)))
    messages = link.poll_status()
    checks.append(("status frame still parsed", messages == [(7, module.STATUS_ACK)]))

    ack = module.build_status_packet(8, module.STATUS_ACK)
    connection.inject(b"noise!" + ack[:1])
    checks.append(("partial header waits", link.poll_status() == []))
    connection.inject(ack[1:])
    checks.append(("ACK after noise and split header", link.poll_status() == [(8, module.STATUS_ACK)]))
    for split in range(1, len(ack)):
        connection.inject(ack[:split])
        first = link.poll_status()
        connection.inject(ack[split:])
        checks.append((f"ACK split at byte {split}", first == [] and link.poll_status() == [(8, module.STATUS_ACK)]))

    # 上位机自己发出的启动通知帧（AA 55 02）可能被自己读到（例如串口回环），
    # 解析器必须忽略它，不能当成 STM32 的按键事件。
    connection.inject(module.build_hello_packet())
    messages = link.poll_status()
    checks.append(("hello frame ignored by parser", messages == []))
    checks.append(("hello frame is not a key press", link.take_key_presses() == 0))

    # 抓取帧（AA 55 开头）不应被当成本模块的接收帧
    pick_body = bytes((0xAA, 0x55, 0x01, 1, 5, 10, 0, 20, 0))
    connection.inject(pick_body + bytes((sum(pick_body) & 0xFF,)))
    messages = link.poll_status()
    checks.append(("off-direction pick frame ignored", messages == []))

    # ---- 2. 启动通知帧的构造 ----
    hello = module.build_hello_packet(3)
    checks.append(("hello frame layout", hello[:5] == bytes((0xAA, 0x55, 0x02, 3, 0x01))))
    checks.append(("hello frame checksum", hello[5] == (sum(hello[:5]) & 0xFF)))

    # 下位机的“本机在线”告知（55 AA 02）只置一个日志标志，不产生消息。
    board_body = bytes((0x55, 0xAA, module.HELLO_FRAME_TYPE, 0, module.HELLO_STATE_READY))
    before_total = link.received_total
    connection.inject(board_body + bytes((sum(board_body) & 0xFF,)))
    messages = link.poll_status()
    checks.append(("board hello sets ready flag", link.board_ready is True))
    checks.append(("board hello yields no status message", messages == []))
    checks.append(("board hello is not a key press", link.take_key_presses() == 0))
    checks.append(("rx byte counter grows", link.received_total - before_total == 6))
    controller = module.PickController(
        serial_link=link,
        stable_frames=5,
        stable_radius_mm=5.0,
        ack_timeout=1.0,
        pick_timeout=24.0,
        empty_confirm_seconds=1.0,
    )
    checks.append(("starts in WAIT_BUTTON", controller.state == "WAIT_BUTTON"))
    checks.append(("starts armed off", controller.waiting_button is True))

    start = time.monotonic()
    controller.update([], start)
    checks.append(("stays in WAIT_BUTTON without a press", controller.state == "WAIT_BUTTON"))

    connection.inject(build_key_frame(9, module.KEY_STATE_PRESS))
    controller.update([], start + 0.05)
    checks.append(("press arms the run", controller.waiting_button is False))
    checks.append(("press enters DETECT", controller.state == "DETECT"))

    # 视野内没有可抓目标：超过 empty_confirm_seconds 后回到等待按键。
    controller.update([], start + 0.30)
    checks.append(("still detecting while confirming", controller.state == "DETECT"))
    controller.update([], start + 1.50)
    checks.append(("no target keeps scanning", controller.state == "DETECT"))
    checks.append(("does not require another press", controller.waiting_button is False))

    # ---- 3. 启动通知只被计一次 ----
    # 直接复用真实的 SerialLink 实例，只把 pyserial 连接换成记录用的替身。
    connection.written.clear()
    link.send_hello()
    written = bytes(connection.written)
    checks.append(("send_hello writes exactly one frame", len(written) == 6))
    checks.append(("send_hello frame is AA 55 02", written[:3] == bytes((0xAA, 0x55, 0x02))))
    checks.append(("send_hello checksum", written[5] == (sum(written[:5]) & 0xFF)))

    link.last_hello_time = time.monotonic() - module.HOST_HEARTBEAT_PERIOD_SECONDS
    connection.written.clear()
    link.send_hello_if_due()
    checks.append(("hello heartbeat writes after one period", len(connection.written) == 6))
    connection.written.clear()
    link.send_hello_if_due()
    checks.append(("hello heartbeat is rate limited", len(connection.written) == 0))

    print("=== checks ===")
    failed = 0
    for name, ok in checks:
        print(f"{'PASS' if ok else 'FAIL'}  {name}")
        failed += 0 if ok else 1
    print("ALL CHECKS PASSED" if failed == 0 else f"{failed} CHECK(S) FAILED")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
