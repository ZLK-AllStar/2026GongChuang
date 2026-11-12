# TRAY POS READ 现场排查（2026-11-08）

## 现场结论

重新 SSH 登录 `jetson@192.168.100.15` 后，确认 `huatai-vision.service`
启动 `/home/jetson/classic/cv2_copy.py`，工作目录为 `/home/jetson/classic`。
检查时服务为 `active/running`、`NRestarts=0`，进程持有 `/dev/video0` 和
`/dev/ttyTHS1`，串口没有其他进程占用。标定文件包含本次提供的四组点。

用户确认屏幕错误为 `TRAY POS READ`，原点指示为 `1/0`；未确认失败前托盘是否转动。

Jetson journal 中的关键记录（北京时间）：

```text
16:42:36.144377 TX pick: seq=1 cmd=84 xy=(101.3,47.4)
16:42:36.159270 RX status: seq=1 status=ACK
16:42:49.903643 RX status: seq=1 status=DONE
16:42:50.412500 TX pick: seq=2 cmd=19 xy=(172.8,96.7)
16:42:50.426075 RX status: seq=2 status=ACK
16:42:50.526734 RX status: seq=2 status=FAILED
```

第一单耗时约 13.76 秒完成；第二单在 ACK 后约 101 毫秒失败。
后续新任务直接返回 FAILED，未再收到 ACK。未发现 Python 异常或抓取超时。

对照当前固件：第二个新类别被分配 ID2，托盘目标相位为 60°。
`Tray_MoveToAngle()` 的位置读取（0x36）或移动前状态读取（0x3A）失败，
都可能返回 `TRAY_RESULT_POSITION_READ_ERROR`，显示 `TRAY POS READ`。
`Tray_Abort()` 随即停止托盘并将托盘原点置为无效；后续 `OnFrame()` 在
原点检查处直接返回 FAILED。因此当前状态不能通过继续重发任务恢复。

现有 HOST 协议只返回通用 FAILED，没有电机总线原始回包，尚不能区分
真实未回包、收到损坏帧或前台超时误判，也不能由这些日志断定托盘是否已转动。

## 已修复的软件缺陷

`UART_RxPopFrame()` 等待回包时调用 `AppRuntime_Service()`，其中包含阻塞式屏幕刷新。
旧代码在服务返回后直接检查经过时间，没有重新检查 ISR 是否已经将回包入队。
服务耗时超过 50 ms 时，即使有效回包早已到达，也会返回超时。

使用实际 `usart.c` USER CODE 区离线编译复现：模拟 5 ms 时收到地址 3 的
有效位置帧，服务到 65 ms 才返回，旧代码对 50 ms 等待返回 false，队列仍有一帧。
这证明该路径存在软件缺陷；5/65 ms 是模拟条件，不是本次板上实测耗时。

修复在超时返回前再次检查队列，已入队的帧优先交给调用者解析。
没有回包时仍按原超时失败，帧格式校验、原点保护及 24 秒任务预算继续生效。
只修改 `Core/Src/usart.c` 的 USER CODE 区，未修改外设生成初始化或 `.ioc`。

新增 `tools/selftest_uart_queue.py` 从实际 C 文件提取队列实现，验证慢服务期间到帧、
无帧超时、SysTick 回绕、FIFO 顺序和零等待读取。

## 验证与交付状态

```text
python -X utf8 tools/selftest_uart_queue.py        PASS
python -X utf8 tools/selftest_host_link.py         PASS
python -X utf8 tools/selftest_mechanical.py        ALL MECHANICAL REGRESSIONS PASSED

BUILD_RESULT: SUCCESS
ERROR_COUNT: 0
WARNING_COUNT: 0
ERRORS: none
WARNINGS: none
FULL_LOG: MDK-ARM/build_tray_feedback_20261108.log
```

使用 ARMCC 5.06 update 7 build 960，对 HUATAI target 执行 `-r` 全量重建；
链接成功，并生成 `MDK-ARM/HUATAI/HUATAI.hex`。

尚未烧录。SSH 排查期间未重启视觉服务，未通过串口注入额外抓取、清零或运动指令。
当前实机的失败状态仍然存在，不能把离线通过视为现场恢复。

烧录后需在预期机械原点初始化，释放再新按 PE2，复测跨类别切换。
如仍失败，下一项证据应为地址 3 的 0x36/0x3A 原始反馈和接收耗时；
在回包确认前，不应通过绕过原点保护或 HOST 自动清零恢复运动。
