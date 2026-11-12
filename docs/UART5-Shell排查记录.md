# DEBUG / UART5 Shell 无波形排查

检查日期：2026-11-01。对象为当前工作区源码，未连接目标板验证，未修改固件源码或下载程序。

## 结论与验证

没有发现 UART5 引脚、外设时钟、115200 8N1 配置、中断向量或 Shell 收发分发缺失的问题。不能仅凭编译和模拟测试认定实机软件运行正常，也不能仅凭空闲时没有跳变认定硬件损坏。

- BUILD_RESULT: SUCCESS
- ERROR_COUNT: 0
- WARNING_COUNT: 0
- ERRORS: 无
- WARNINGS: 无
- FULL_LOG: `C:\Users\ZLK\AppData\Local\Temp\HUATAI-uart5-20261101-220621.log`
- 工具链：ARMCC V5.06 update 7 (build 960)，target HUATAI，使用 `-r` 全量重建并成功链接、生成 HEX。
- `python -X utf8 tools/selftest_mechanical.py` 全部通过，包括实际 Shell 库/命令/端口、模式隔离、UART 错误恢复、启动按键以及 2435 项 CoreXY 检查。HAL/UART 为模拟，不能证明板上电平和中断正常。
- 本次生成的 `MDK-ARM/HUATAI/HUATAI.map` 确认向量引用 `stm32f4xx_it.o` 的 `UART5_IRQHandler`，接收回调引用 `host_link.o`，命令表引用 `shell_commands.o`。

## 软件检查要点

| 路径 | 检查结果 |
| --- | --- |
| `Core/Src/usart.c:47` | UART5，115200、8 数据位、1 停止位、无校验、无流控、16 倍采样、收发均启用 |
| `Core/Src/usart.c:162` | UART5/GPIOC/GPIOD 时钟开启；PC12=TX、PD2=RX，均为 AF8；没有发现应用后续重新占用这两个引脚 |
| `Core/Src/shell_port.c:14` | TX 使用 `HAL_UART_Transmit(&huart5, ..., 100)` 阻塞发送；启动输出不依赖 UART5 中断 |
| `Core/Src/shell_port.c:24` | 仅 DEBUG 初始化；设置写接口、初始化命令库、开启 NVIC、启动单字节中断接收、输出启动说明 |
| `Core/Src/stm32f4xx_it.c:272` | UART5 IRQ 调用 `HAL_UART_IRQHandler(&huart5)` |
| `Core/Src/host_link.c:372` | 唯一全局接收回调按 Instance 分发 UART5 到 Shell；错误回调也有 UART5 分支 |
| `Core/Src/main.c:143` | DEBUG 主循环调用 `ShellPort_Poll()` |
| `Core/Src/shell_commands.c:151` | 默认 letter 用户无密码；支持 CR 或 LF 回车；有 help、status 等命令 |

必须考虑的运行条件：

1. DEBUG 是应用运行模式，不是 Keil 的 Debug 调试会话。启动 3 秒内 PE2 稳定低电平 20 ms 就锁定 HOST，直到复位。DEBUG 应为 LED0/PF9 灭、LED1/PF10 亮，屏幕显示 DEBUG。
2. Shell 位于全部应用初始化之后（`main.c:121–133`）。仅固定等待已有约 7.24 秒：模式选择 3 秒、屏幕复位 0.24 秒、真空 0.5 秒、升降 2+1 秒、XY 就绪 0.5 秒；还需加屏幕刷新和电机事务时间。建议从复位前开始采集，正常条件下至少留 15 秒观察窗口，该时间不是故障情况下的启动上限。
3. 普通电机无应答有超时返回，原点无效本身不会禁止 DEBUG Shell。若程序进入 Error_Handler/HardFault 或 SysTick 停止，仍可能到不了 Shell。
4. 屏幕发送每次最长等待 1000 ms，刷新失败后继续发送后续块；整屏有 720 个像素块。如果 SPI 持续超时，初始化及服务钩子可能被严重拖慢。这是现有软件的诊断可用性隐患，尚无实机证据证明它是本次根因。屏幕未接本身不等于 SPI 超时，因为 SPI 发送不要求屏幕 ACK。
5. 启动提示只发一次，没有周期串口心跳。之后必须发送字符/回车才有回显。PD2 是输入，MCU 不会主动在 PD2 上输出启动波形。
6. PD2 当前 GPIO_NOPULL，USB-TTL 未接时可能悬空；接收错误会使当前行被丢弃，需要再按回车后重新输入。接收启动返回值未记录，必要时由调试器确认 HAL_OK。这些问题不能单独解释正常执行到阻塞 TX 时 PC12 完全没有输出。

## 先做不改代码的实机定位

1. 打开正确的 HUATAI 工程，核对下载的是本次构建镜像。进入调试后必须 Run；不要一直停在 main。上电默认会动作升降机构，检查时留出行程。
2. 不按 PE2 复位，确认 DEBUG 灯态和屏幕模式；PE2 未按时应为高电平。
3. 在 `ShellPort_Init`、`shell_port.c` 的 `Write`、`Error_Handler`、`HardFault_Handler` 设置断点。若到不了 Shell，暂停查看调用栈，先定位卡在何处；不要先归因 USB 串口。
4. 若到达 Write，在发送前布置好 PC12 采集，然后运行。初始欢迎文字使用阻塞 TX，即使接收 IRQ 有问题也应能看到 TX。检查 `HAL_UART_Transmit` 返回值；不是 HAL_OK 时先检查 huart5 状态及寄存器。
5. 若 TX 正常，电脑发送 `status` 加 CR；依次检查 `UART5_IRQHandler`、`ShellPort_RxComplete` 和主循环 `ShellPort_Poll`。不要持续暂停单步同时连续发数据，以免人为造成 ORE。

寄存器参考（168 MHz 系统时钟、APB1=42 MHz，初始化完成后）：

| 项目 | 预期 |
| --- | --- |
| `RCC->APB1ENR` bit 20 | UART5EN=1 |
| `RCC->AHB1ENR` bit 2/3 | GPIOCEN/GPIODEN=1 |
| `(GPIOC->MODER >> 24) & 3` | 2，PC12 复用模式 |
| `(GPIOC->AFR[1] >> 16) & 15` | 8 |
| `(GPIOD->MODER >> 4) & 3` | 2，PD2 复用模式 |
| `(GPIOD->AFR[0] >> 8) & 15` | 8 |
| `UART5->BRR` | 0x16D，对应约 115068 baud |
| `UART5->CR1` | UE/TE/RE=1，M/PCE/OVER8=0；Shell 接收启动后 RXNEIE=1 |
| `UART5->CR2` / `CR3` | STOP=00；RTSE/CTSE=0 |
| `NVIC->ISER[1]` bit 21 | UART5 IRQ 53 已开启 |
| `huart5.gState` / `RxState` | 空闲发送时 READY；已挂接单字节接收时 BUSY_RX |

读取 UART SR/DR 可能清除接收及错误标志，避免调试器持续刷新 DR 干扰接收定位。

## 硬件检查顺序

1. **USB-TTL 自环。** 与主板断开，短接转换器 TX/RX，关闭终端本地回显，115200 8N1、无流控。发送文本应原样收到。失败先查 COM 口、驱动、线缆或转换器。电脑串口 TX 连续发送十六进制 55 时，逻辑分析仪应看到跳变。
2. **明确接线。** 使用 3.3 V TTL 信号：转换器 TX → PD2、转换器 RX ← PC12、转换器 GND ↔ 主板 GND ↔ 分析仪 GND。不要接 RS-232 正负电压接口。单独给主板供电时无需接转换器 VCC；转换器电源跳帽不一定改变 TX 逻辑电平，要实测。
3. **核对真实引脚。** 按板卡原理图确认排针确实连到 MCU PC12/PD2，不要把 PC2/PD12 混淆，也不要默认板载 USB 串口连接 UART5。检查这两脚是否还连接 SD 卡、其他模块、跳帽或电平转换器；PC12/PD2 可用于 SDIO，板上共享线路可能加载信号。断电后测排针到对应焊盘连续性及对地短路。
4. **测静态电平。** 程序运行到 UART5 初始化后，PC12 空闲通常应接近 3.3 V；连接正常且空闲的 USB-TTL 时，PD2 也应接近 3.3 V。一直为 0 V 时检查短路、外设拉低、未供电和未运行到初始化。PD2 未接时是无上下拉输入，不能据其电平判坏。
5. **重新抓取。** CH0 接 MCU 侧 PC12，CH1 接 MCU 侧 PD2，共地；采样率推荐 10 MHz 或更高，UART 解码 115200 8N1、非反相、LSB first。PC12 下降沿触发，复位前开始等待，保存足够长的数据。每位约 8.68 us；启动后发送回车，再发送 `status\r` 或 `help\r`。
6. **分段比较。** 同时在转换器 TX 与 MCU 侧 PD2 测试。转换器有跳变、PD2 无跳变：查接线/排针/跳帽。PD2 有正确数据而 PC12 不回显：回到模式、程序进度、中断及错误计数检查。PC12 有输出而电脑不显示：查转换器 RX、COM 口、终端设置。发送执行成功但 PC12 始终无跳变：复核 AF/端口时钟、探测位置及外部负载，再评估引脚损坏。

先测回车、help、status，避免用 move/tray/zero 等机械命令验证串口。持续 0x55 只用于转换器与接收线路电气测试，不作为 Shell 命令；完成后复位并改用短文本命令。
