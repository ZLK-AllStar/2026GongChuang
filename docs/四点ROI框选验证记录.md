# 四点 ROI 框选验证记录

日期：2026-11-09。

## 行为

- 识别窗口按 `R`，顺时针或逆时针依次点击四角，自动连线闭合。
- `Enter` / 空格确认；右键 / 退格撤回；`R` 清空；`Esc` / `Q` / 关闭选点窗口取消。
- 拒绝交叉、重复、非凸或过小的四边形；取消保留原 ROI。
- 新配置为 version 3，保存原像素与归一化四点；继续支持 version 2 矩形。
- 模型输入屏蔽多边形外像素；颜色掩膜也受多边形限制，区域外检测中心不进入抓取目标。
- 只用外接矩形裁图，不作透视变换，原始像素及机械标定坐标保持一致。
- WAIT_ACK / PICKING 时暂不允许选点；选点期间继续串口心跳和状态服务，不选择新抓取目标。

## 文件

本地更新 `上位机脚本/cv2_copy.py`、`上位机脚本/_cv2 copy.py`、`classic-new/_cv2.py`。
Jetson 更新 `/home/jetson/classic/cv2_copy.py` 和 `/home/jetson/classic/_cv2.py`。
操作说明位于 `上位机脚本/摄像头识别界面使用说明.md`，也已上传到 classic 目录。

## 验证与部署

- 本地 `python -X utf8 tools/selftest_roi.py`：三份程序共 200 项检查通过，包含模拟点击、撤回、取消、关闭窗口、奇数分辨率映射、配置缩放、区域过滤及原坐标编码。
- 本地与 Jetson 暂存版 `selftest_cv2_copy.py`：各 1684 项通过，覆盖优先级、24 秒等待、X=215 / Y=205 与 Y 上限限幅。
- Jetson 暂存版 `selftest_roi.py`：部署的两份程序共 127 项通过。
- 本地与 Jetson `selftest_button_logic.py`：通过。所有回归使用替身，不打开实际摄像头或 UART。
- 选点界面用已有摄像头帧渲染并检查；实际鼠标操作仍由用户在 Jetson 桌面执行。
- 备份及暂存验证日志：`/home/jetson/classic/backups/20261109-172505-four-point-roi/`。
- 本地证据：`MDK-ARM/MechanicalVerify/roi-four-points-20261109/`。
- 仅在原服务等待 PE2、尚未开始识别的状态重启。新 PID 18801，active/running；模型、标定、UART、原 ROI 加载成功，停在 WAIT_BUTTON。

部署没有覆盖 ROI 和标定配置，原 ROI 仍为 `(279,157,792,563)`，等待用户重新选四点。
ROI SHA256：`5cc2609b52c26d8677dcc4f3904b1b55a2529ccb9981f7fb734face046005181`。
标定 SHA256：`d6fe1a846c90fe49e5820cca049cdd3ca4dfc56fadbb9c85efcaee690bf63671`。
主程序 SHA256：`adff985a55ade315454fc7538c5cb3840eb5b876a3910c7473cc306c52d389f7`。
