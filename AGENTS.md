# 仓库指南

## 项目概述

HUATAI 是基于 STM32F407ZGT6 的 XY 贴片机固件。通过 USART1 驱动 CoreXY 双电机平台和六槽旋转托盘，用舵机 PWM 控制真空吸取与释放机构，并通过 SPI1 在横向 ST7789 320x240 屏幕上渲染调试状态，USART3 ApusIDE HMI 显示分类记录。帧缓冲为内部 SRAM 的 76800 字节半屏缓冲，屏幕硬件初始化由应用层完成；项目其余基础外设来自 STM32CubeMX，使用 Keil MDK-ARM（uVision5）编译。

## 项目结构与模块组织

```
HUATAI/
├── HUATAI.ioc              CubeMX 工程配置（请勿手动编辑）
├── Core/
│   ├── Inc/                头文件（CubeMX 生成 + 应用层）
│   └── Src/                源文件（CubeMX 生成 + 应用层）
├── Drivers/
│   ├── CMSIS/              ARM CMSIS 核心头文件
│   └── STM32F4xx_HAL_Driver/  ST HAL 库源码
├── MDK-ARM/
│   ├── HUATAI.uvprojx      Keil 工程文件
│   └── startup_stm32f407xx.s
└── BUJIN_motor/            参考工程（F103C8T6，移植来源）
```

应用模块与 CubeMX 文件同位于 `Core/` 目录：

- `screen.c/h` — 屏幕 `write` 抽象接口、ST7789 初始化和阻塞式刷新
- `ext_sram.c/h` — 内部半屏帧缓冲（兼容文件名，ExtSram_Init 不配置 FSMC）
- `tft_lcd.c/h` — 帧缓冲绘图、文字、几何体缩略图
- `app_display.c/h` — 调试页、坐标/原点/真空/托盘/通信状态，最多 10 Hz 刷新
- `button.c/h` — PE2 实体按键（上拉、低电平有效）采样与 20 ms 消抖
- `host_link.c/h` — USART2 上位机链路：抓取帧接收、状态帧与按键事件帧上报
- `xy_stage.c/h` — CoreXY 正反解、双电机同步及原点有效性
- `vacuum.c/h` — TIM1_CH1/CH2、PE9/PE11 泵与阀 PWM
- `aux_servo.c/h` — TIM3_CH1、PA6 升降吸盘
- `tray_motor.c/h` — 地址 3，固定单方向累计绝对角度
- `board_io.c/h`、`app_mode.c/h` — PF9/PF10 低有效 LED，开机 3 秒模式选择
- `app_controller.c/h`、`classification.c/h` — HOST 任务、六槽循环 ID、去重
- `app_runtime.c/h` — SysTick 等待服务与 24 秒任务预算；服务钩子不执行 Shell
- `shell_port.c/h`、`shell_commands.c` — DEBUG 调试 Shell，保留行损坏与 busy 输入保护
- `soft_uart.c/h` — PF2=TX、PF4=RX，4800 8N1；TIM13 8 倍采样，全双工软件串口
- `app_config.h` — Release/HOME、传动参数、缓冲和时间参数唯一来源

## 构建、测试与开发命令

本项目为裸机固件，target `HUATAI`，保持 ARMCC 5.06 update 7 build 960。IDE 编译/烧录：

1. 在 Keil 中打开 `MDK-ARM/HUATAI.uvprojx`。
2. 按 **F7** 编译（Project → Build Target）。
3. 按 **F8** 下载到目标板（Flash → Download）。

命令行用 `D:\Keil_v5\UV4\UV4.exe -b <绝对 uvprojx 路径> -t HUATAI -j0 -o <绝对日志路径>`；最终用 `-r` 全量重建。C 源变更使用 `mdk-armclang-build-check` 技能，以日志终结的 Error/Warning 及链接结果为准。不要仅用 `$LASTEXITCODE`。

自动回归：`python -X utf8 tools/selftest_mechanical.py`（GCC/G++），`python -X utf8 tools/selftest_host_link.py`，`python classic-new/scripts/selftest_button_logic.py`。HMI ARM 构建：`python -X utf8 tools/build_hmi.py`；HMI 收包/线程边界回归：`python -X utf8 tools/selftest_hmi_worker.py`；HMI 测试帧生成：`python -X utf8 tools/hmi_frame_test.py`；HMI 列表表头图重绘：`python -X utf8 tools/make_hmi_header.py`。模拟通过不代表实机方向/精度/耗时通过；首次上机见 `docs/调试与首次上机.md`。

## 编码风格与命名规范

- 语言：C99，使用 ARM Compiler 5/6 编译（`uC99=1`，`useUlib=0`）。
- 缩进：2 个空格。函数定义大括号另起一行（Allman 风格），函数内部 K&R 风格。
- CubeMX 生成的文件包含 `/* USER CODE BEGIN */` / `/* USER CODE END */` 标记。在生成文件中只能编辑标记之间的代码——其余部分在重新生成时会被覆盖。
- 应用文件函数用 `PascalCase`（`TFT_Init`、`XY_MoveTo`），宏/定义用 `大写蛇形`（`TFT_BLACK`、`VACUUM_PICK_MS`）。
- 引脚和外设句柄使用 CubeMX 命名：`CS_Pin`、`hspi1`、`htim2`、`huart1`。

## CubeMX 配置规则

**禁止**手动修改 `.ioc` 文件或从 CubeMX 重新生成，除非获得明确许可。外设的运行时重配（如调整 TIM 预分频/ARR）应放在应用层的 `USER CODE` 区，不要写入 CubeMX 生成的初始化函数。

## 提交与 Pull Request 规范

本仓库已有 Git 历史。提交请遵循以下规范：

- 使用约定式提交前缀：`feat:`、`fix:`、`refactor:`、`docs:`、`chore:`。
- 在标题中注明受影响的模块，例如 `feat(vacuum): 校核 TIM1 的 50Hz 舵机 PWM`。
- 标题行不超过 72 个字符；非简单改动需在正文说明做了什么以及为什么。
- 不要提交构建产物（`*.o`、`*.axf`、`*.hex`、`HUATAI/` 输出目录）。

## 架构说明

上电最小 GPIO 初始化后，LED0（PF9）按 100 ms 半周期快闪完整 3 秒。PE2 在窗口结束前稳定按下 20 ms 选择 DEBUG，否则 HOST，模式锁定至复位。HOST：LED0 亮、LED1 灭；DEBUG：LED0 灭、LED1（PF10）亮。屏幕/舵机等待发生在选择完成之后；TIM13 仅 DEBUG Shell 使用（84 MHz、PSC=0、ARR=2187，约 38.391 kHz 更新中断），HOST 不启动。应用毫秒时基仍为 HAL_GetTick/SysTick。

XY（地址 1/2）和托盘（地址 3）以本次上电机械位置建立软件原点，不寻找限位或主动回机械原点。使能/清零应答和反馈不成功时禁止 HOST 运动，DEBUG 仍可诊断。CoreXY 为 P1=(X+Y)*80、P2=(X-Y)*80，范围 X=0..215、Y=0..205 mm。方向宏同时用于命令和反馈，必须低速实测。串口点到点联动不保证任意斜线严格直线插补。

HOST 初始化完成后须先稳定释放 PE2，再新按一次；原 USART2 字节协议和状态码保持不变；Jetson 的 classic/cv2_copy.py 每轮按长方体、正方体、五棱柱、圆柱、圆锥、三棱锥的顺序选择有效目标，同类沿用空旷度/移动距离评分。上位机转换后的 Y 大于 MACHINE_Y_MAX_MM 时直接截到上限 205 mm，显示、目标筛选和串口发送均使用限幅坐标；下位机仍拒绝直接收到的越界帧。收到合法任务后先 ACK，为新 code 首次分配 ID1..ID6，第七新类循环到 ID1，旧类仍记住原 ID。先抬杯、转托盘至 (ID-1)*60°，再去原始抓取坐标、吸取抬杯、去固定 Release、保持抬杯释放、发 HMI 记录、停靠 HOME；停靠失败报 FAILED。Release/HOME 初值分别为 (0,0)，均在 app_config.h 配置；删除旧减 10 mm 补偿和物料盒坐标生产路径。重复完整任务键 (SEQ,CMD,X_raw,Y_raw) 只补状态，不重做机械动作/记录。

USART1 保留本工程 DMA+IDLE/Emm_V5；三台电机串行使用同一事务总线。USART2 使用字节中断环，唯一全局 UART 回调仅处理其 HOST 链路。UART5 的生成初始化保留，但不再收发或分发 Shell。Shell 改用 PF2=TX/PF4=RX、4800 8N1、3.3 V TTL；软件串口 ISR 只推进位状态机与 RX 入队。TX 环 512 字节，满时前台有界等待并服务 AppRuntime；RX 256 字节、编辑/历史缓冲 512 字节。帧错误、失步、溢出废弃整行，跨 busy 边界的在途帧通过输入代际作废。TIM8/TIM13 共享 IRQ 按来源分发，DWT CYCCNT/TIM13 CNT 记录失步诊断；不得将其计数宣称为完整 IRQ 实测。HAL_Delay 服务钩子只做显示和 HOST 解析/状态，绝不重入机械或 Shell。同步长动作不提供即时软件急停。接线、HSI 漂移和最坏中断时延仍待实机验收，见 docs/PF2-PF4软件串口Shell验证记录.md。

托盘采用 3200 pulse/rev、65536 反馈单位/rev、传动比初值 1.0；固定方向，不走最短路径。300°→0° 为继续转 60°；累计目标整体取整，防止六步一圈漂移。方向、传动比、零槽和容差仍需实测。错误使原点失效，HOST 不自动清零；DEBUG 的 zero xy/zero tray 仅重新建立当前位置软件原点。

真空使用 TIM1（168 MHz，PSC=167、ARR=19999）；升降使用 TIM3（84 MHz，PSC=83、ARR=19999），均 1 us/tick、50 Hz。先写安全 CCR/更新事件，再启动 PWM。保留吸取 1500 ms、释放 2000 ms 和升降稳定时间，释放不新增降杯。圆柱系列（全部颜色）取料使用独立的 AUX_SERVO_LIFT_DOWN_ANGLE_CYLINDER=130°；其余形状沿用各自下降预设。取料后运动失败保留受控真空，不盲目泄压。

ST7789 保留阻塞 SPI 与内部半屏缓冲，只作调试。HMI 在 USART3 PB10/PB11、115200 8N1；原 1..72 单字节仅预览，新 22 字节 F0/高位半字节/CRC/F1 帧携带固件 ID 与记录序号。无 HMI ACK，发送失败计数但不重抓。指定 HMI 为 D:\BaiduNetdiskDownload\ApusIDE2_v20250930\ApusIDE2_v20250930\workspace\TEST；仓库 TEST/ 副本不是交付目标。

本地启动汇编仅调整应用可配置 Stack_Size 为 0x1000（4 KB），未迁移参考工程启动/底层初始化。文档、验证记录及后续实机矩阵见 docs/机械架构变更执行方案.md 与 docs/机械架构变更验证记录.md。HMI 屏（ApusIDE 工程）的调试方法与 SDK 已确认行为见 docs/ApusIDE-HMI调试手册.md。
