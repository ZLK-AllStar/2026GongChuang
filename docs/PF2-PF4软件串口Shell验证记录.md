# PF2/PF4 软件串口 Shell 验证记录

日期：2026-11-02（Asia/Shanghai）。结论：固件代码落地、离线回归与 ARMCC 5 全量重建通过；未烧录，实机接线、时序与并发负载验收待执行，尚未实机放行。

## 实施范围

基线为 HEAD `95c1662` 上实施前的当前工作区，保留已有未提交修改；本次未创建提交。实施前差异及相关文件副本存于本地忽略目录 `MDK-ARM/SoftUartVerify/baseline/`，不将其他机械架构改动归为本次新增。

| 文件 | 本次改动 |
|---|---|
| `Core/Inc/app_config.h` | PF2/PF4、4800 baud、8 倍采样、84/168 MHz 时钟校验、512 字节 TX 环、发送余量、10 μs 抖动诊断阈值 |
| `Core/Inc/soft_uart.h`、`Core/Src/soft_uart.c` | 独立 RX/TX 状态机；GPIO IDR/BSRR；TIM13 PSC=0/ARR=2187；DWT 回绕处理、间隔/相位失步检测；背压、短写和诊断接口 |
| `Core/Inc/shell_port.h`、`Core/Src/shell_port.c` | 后端切换；先启动 TX 再初始化 Letter Shell；完整字节/错误入口；busy 输入代际；每次消费重新检查损坏标志 |
| `Core/Src/shell_commands.c` | `status` 增加收发、帧错、假起始、失步、发送故障/超时及间隔/相位峰值 |
| `Core/Src/stm32f4xx_it.c` | 仅 USER CODE 区加入 TIM13 快速分支；TIM8 更新来源独立处理，软件后端运行时避开重复 HAL TIM13 路径 |
| `Core/Src/host_link.c` | 删除 UART5 的 Shell 回调分发，保留 USART2 链路 |
| `MDK-ARM/HUATAI.uvprojx` | 仅新增 `soft_uart.c` 工程条目 |
| `tests/mechanical/`、`tools/selftest_mechanical.py` | 更新 Shell 替身，新增位时序和从定时中断到真实 Letter Shell 的集成测试 |
| `AGENTS.md`、`docs/调试与首次上机.md`、原实施方案 | 同步当前接线、波特率、TIM13 用途和验收状态 |

未改 `.ioc`，未运行 CubeMX。逐段比对实施前副本，`stm32f4xx_it.c` 的生成区一致；`main.c` 字节内容一致，原 DEBUG/HOST 初始化入口直接复用。Keil XML 对比确认除新增驱动条目外配置一致，编译器未迁移。未改 `tim.c/gpio.c/usart.c/dma.c`，未调整 USART1/DMA 优先级或接收临界区。

## 行为与诊断

- DEBUG 才配置 PF2/PF4 并启动 TIM13；HOST 不调用后端初始化，驱动本身也检查模式。UART5 生成初始化保留，Shell 不启用其 RX IT、不向其发送、不接纳其回调。
- RX 在检测到起始低电平后第 4 拍确认起始、第 12 拍采样 D0、第 76 拍检查停止位。正常停止后立即恢复起始检测；启动/帧错后需连续 8 拍高电平，持续低电平不会重复交付零字节。
- TX 每位 8 拍，停止位满 8 拍才进入下一帧。调用者数据复制进可用 511 字节的 TX 环，返回只表示接受长度。满时服务 `AppRuntime_Service()`，不解析 Shell 或派发机械任务。
- 发送等待上限按实际分频器推算“已有队列 + 本次数据 + 在途与恢复共两帧”的完整线速时间，向上取整，再加 250 ms（两个 100 ms 显示服务周期加 50 ms 调度余量）。从 ISR 或关中断上下文调用直接拒绝；队列发布仅屏蔽一个字节的存储和索引更新。
- DWT 相邻服务间隔大于一拍加 10 μs，或 TIM13 CNT 相位大于 10 μs，记失步、废弃接收行；发送中/有待发数据时另记发送故障并取消队列，TX 保持至少一帧高电平。不会补跑不存在的采样点。阈值是故障诊断策略，不能证明完整时延满足要求。
- 输入代际在忙碌开始/结束及 Shell 就绪时改变；一帧在起始检测时保存代际，跨边界完成的字节不能进入可执行行。`EndAction` 同时检查 RX 在途状态/线路电平并清 RX 队列，不重置公共定时器或 TX 相位。
- `status` 的 `tx` 只计本地完整发送帧，无远端确认。`maxgap/maxlate` 分别以 CPU 周期/TIM13 tick 记录，快照逐字段读取；不表示同一时刻的原子快照，也不代表完整 IRQ 耗时实测。短写会累计 Shell 错误，但库并非所有调用点都会重试，异常后文本可能残缺。
- ISR 接收错误和前台发送/行长度错误分别计数后求和，避免跨中断读改写同一个计数器时丢失增量；中断与前台共享的就绪、收发和队列状态使用 `volatile`，busy 边界及单字节发布保持短临界区。

## 离线验证

实施前：三项原有回归均通过；Keil 基线增量检查为 0 Error、0 Warning。旧增量产物不能作为本次完整功能或内存基线，以下内存以最终全量链接为准。

| 验证 | 实际结果 |
|---|---|
| `python -X utf8 tools/selftest_mechanical.py` | 通过，包含下列新测试及原托盘、启动/按键、HOST 10 场景、2435 项 CoreXY 检查、指定外部 HMI 实际解析器 |
| `test_soft_uart.c`：真实驱动与共享 IRQ，虚拟 GPIO/DWT/TIM | 通过：256 种 TX 位序/每位宽度、256 种 RX、无间隔全双工；8 种节拍内起始相位、对端位时间 -2%/0/+2%；假起始、坏停止位、持续低、恢复；DWT 回绕、丢拍和相位超限；TIM8/TIM13 来源/UIE/单次处理；TX 环回绕/满/背压、超时与毫秒回绕、临时缓冲覆盖、ISR/关中断拒绝、HOST 与时钟检查 |
| `test_shell.c`：实际命令/端口/Letter Shell，软件后端替身 | 通过：原命令和参数校验、受损行、busy 代际、输出短写统计、错误发生于回显期间、长输出不重入、模式保护 |
| `test_shell_wire.c`：真实驱动/IRQ/端口/Letter Shell，计数测试命令 | 通过：busy 开始/结束发生于起始位、数据位、停止位；残余 `tray 60` 不执行；帧错/失步后整行丢弃；长输出期间环溢出且已排队合法命令也不执行；换行后重输可恢复、无 Shell 重入 |
| `python -X utf8 tools/selftest_host_link.py` | 通过 |
| `python classic-new/scripts/selftest_button_logic.py` | 通过 |

模拟仅验证软件状态和输入/输出断言，不是目标板误码率、HSI 时钟精度或最坏 ISR 时延证明。机械回归继续使用原 HAL/UART 替身，不能代表实机机械方向和耗时通过。

## 最终 Keil 重建

调用 `D:\Keil_v5\UV4\UV4.exe -r <绝对工程路径> -t HUATAI -j0 -o <绝对日志路径>`。日志确认 `Rebuild target 'HUATAI'`，编译器为 ARMCC **V5.06 update 7 (build 960)**，链接及 HEX 生成完成。

```text
BUILD_RESULT: SUCCESS
ERROR_COUNT: 0
WARNING_COUNT: 0
ERRORS: none
WARNINGS: none
FULL_LOG: MDK-ARM/SoftUartVerify/rebuild.log
Code=48728 RO-data=3028 RW-data=444 ZI-data=89260
```

链接映射 `MDK-ARM/HUATAI/HUATAI.map`：RW+ZI 共 **89704 字节**（约 87.60 KiB），`RW_IRAM1` 占 `0x15E68`/`0x1C000`，剩余 **24984 字节**；`RW_IRAM2` 的 16 KiB 未使用。76800 字节内部半屏缓冲保留，栈 4096 字节和堆 512 字节已包含于上述链接占用。静态链接有余量，运行时栈高水位和中断嵌套余量尚未实测。产物、日志及实施前快照均位于已忽略的 MDK 输出子目录，不提交。

## 上机待验收

按原方案第 9.2 节执行，以下均为**未验证**：PF2/PF4 导通、板载负载/接线、3.3 V 电平和复位空闲；HSI/实际位宽与漂移；每方向至少 100000 字节及全双工误码；窄脉冲/断线/忙碌边界/溢出的实际恢复；完整 IRQ 耗时、最坏事件到采样延迟、全局关中断区及 USART1 满 255 字节异常帧开销；屏幕与电机/DMA 并发；实机 HOST/机械回归及栈高水位。

原始长流量测试必须用不接机械命令的独立测试固件调用软件串口接口；本次新增的是主机虚拟外设测试，未交付或烧录实机原始流量测试镜像，不把随机二进制直接送入生产 Shell。若时延超预算，先记录诊断和波形，再决定是否调整优先级或重设计电机队列临界区，并重跑电机回归。

接线和终端：PF2→适配器 RXD，PF4←适配器 TXD，GND 共地；4800 8N1、无流控、关闭本地回显。一条命令一次，待输出完成；RX 环可用 255 字节，不能保证连续批量粘贴可靠。未达到实机矩阵条件前，不标记为“实机 DEBUG Shell 已验收”。
