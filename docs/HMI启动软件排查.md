# TEST HMI 下载后黑屏/停在启动画面：软件排查

日期：2026-11-02。

## 当前实现状态：回调驱动 + 一次 UI 跳转（2026-11-02 晚，落实方案 A）

指定 TEST 工程的 Wcmn 已删除 STTTimer：`wMCom3_vch_cb` 在 COM2 接收线程用 `readData(bytes, 0)` 零等待取数，`consumeSerial()` 解析成事件，每批通过一次 `AsynWorker::onFinished` 跳到 UI 线程执行 `applyEvents()` 更新 `串口`、`commandHistory` 与 Frm02 列表。接收线程不碰任何 UI 状态，UI 线程不碰 `parser`，因此没有应用层 mutex、队列或轮询。

关键依据：本地反汇编 `STTTimer::onEventHandler` 只接受 `code == 31`（`Event::READY`），其他 code 直接返回；旧的轮询版本注册的是 `Event::VALUE_CHANGED`（28），所以回调从未被登记，`pollSerial` 一次都没执行——那段轮询是死代码，也就不能是黑屏的成因。Windows 模拟器对照：`VALUE_CHANGED` 10 秒 0 次回调，`READY` 11 秒 90 次。

验证：`python -X utf8 tools/selftest_hmi_worker.py`（真实 Wcmn.cpp 方法体 + SDK 替身，替身会用断言强制“UI 写入只发生在 onFinished 内”）通过；`python -X utf8 tools/build_hmi.py` 通过且无警告，新应用库不再导入任何 STTTimer 符号；Windows 模拟器注入真实 22 字节帧，三帧按序落到列表并渲染图片。发帧工具：`python -X utf8 tools/hmi_frame_test.py`。

列表显示另有一个 SDK 行为：`STTList` 在 `clearOptions()` 之后创建的第一个子行不套用列表文字样式，标签为白色，白底列表上首条记录只显示图片不显示文字。`Frm02::syncCommandHistory()` 现用一条只带图片的表头行占住该位置（`./images/HDR-RECORDS.png`，971×41，文字画在图里），既吸收该样式状态又变成表头；部署时需一并拷贝该 PNG。细节与逐项排除记录见指定工程“通信与分类显示说明.md”。

候选库 `MDK-ARM/MechanicalVerify/hmi_arm/libbootpanel.so` 未覆盖工程 Release，也未下载到屏端。修改前备份在 `MDK-ARM/MechanicalVerify/hmi_polling_backup/20261102-202935/`。实机启动与收包仍未验证；黑屏根因尚未取得屏端日志，本次改动同时移除了唯一的 STTTimer 变量依赖，可作为下一轮 A/B 的对照组。

## 当时状态（已被上一节取代）：UI 线程轮询

用户要求不带 mutex 的方案后，指定 TEST 工程的 Wcmn 一度改成 UI 线程 STTTimer 每 125 ms 轮询，readData 等待参数固定为 0。应用层 receiveMutex、deque 和 AsynWorker 已删除；原串口事件回调为空，业务状态只由 UI 定时器访问。每轮最多读取 8 批，只刷新一次列表；退出时停定时器再关串口。公共页注册当前启用。

ARM C++11 编译和链接通过，无编译警告；新增 STTTimer 导入在本地配置的核心库中均存在，新应用库没有 mutex 动态导入。`python -X utf8 tools/selftest_hmi_worker.py` 是其后继回归脚本（原 `selftest_hmi_polling.py` 已删除）。没有进行实机下载或启动验证。该方案后来确认回调事件码登记错误，实际从未运行。

候选库为 `MDK-ARM/MechanicalVerify/hmi_arm/libbootpanel.so`，没有覆盖 HMI Release。修改前文件备份在 `MDK-ARM/MechanicalVerify/hmi_polling_backup/20261102-193320/`。详细机制与限制同步记录在指定工程的“通信与分类显示说明.md”。该方案增加约一个轮询周期的显示等待，不保证持续满速输入无丢包，也没有消除 SDK 内部的锁。

以下章节保留排查过程中的对照与假设；其中“尚未修改”和原始行号描述的是当时状态，不代表当前版本。移除应用层锁不能证明黑屏根因已经解决；若仍失败，应继续定位公共页构造与 STTCom::set/open。

## 新增对照结果：注释公共页注册后可以运行

用户实机反馈：注释 `regCmdWindow(cWnd, new Wcmn(this, nullptr, "Wcmn"));` 后能够启动。当前磁盘文件也已注释此行。本次保留这一修改。

这个对照将首要排查范围缩小到 Wcmn 构造、公共页生命周期、STTCom 初始化及串口事件处理。下文原先列出的全局运行库加载风险降为次要假设：如果只是这行改变、其余构建和部署条件相同，整库缺少动态符号/版本通常不应因不创建公共页而恢复。仍需注意实际二进制是否重链接删除了依赖，不能绝对排除组件局部兼容性。

### 已发现的具体死锁风险

`Wcmn.cpp:61` 获取非递归 `receiveMutex` 后，`Wcmn.cpp:63` 调用 `wMCom3->readData(bytes, 100)`。

本地 20250930 / 1.1.0.0 ARM 核心库反汇编确认：`STTCom::readData` 在缓冲无数据且等待未超时时调用 `SttLooper::currentLooper()`，随后调用 `SttLooper::exec(5)`，再 sleep/retry。也就是说，所谓阻塞读取在等待期间会处理事件。

若该事件循环执行另一串口回调，或执行已排队的 `drainEvents()`（第 82 行也获取 receiveMutex），当前线程会再次申请自己仍持有的非递归锁，形成永久死锁。100 ms 读取超时不能解除这种锁等待。它需要“读取暂时无数据 + 等待期间执行再次取锁的事件”这两个条件；尚未取得屏端调用栈证明实机此次正是此路径。

优先的最小修正方向：保留读取、解析和入队的顺序保护，将事件回调内读取改成 `readData(bytes, 0)`。本地实现无数据时会直接返回，不进入上述事件循环；不要仅改成递归锁来掩盖重入，也不要随意把读取移出锁造成多个读取者打乱解析顺序。此修正尚未应用到用户工程。

### 初始化链路与判别方法

本地 `regCmdWindow` 实现只写入 cWnd 并将页面加入公共页列表；Wcmn 的 createParts/onLoad 等由框架后续初始化。因此“注释注册有效”不能证明注册函数本身坏了，也同时跳过了表达式中的 `new Wcmn`。

`createParts()` 依次创建 STTCom、设置 COM2 参数、open、注册回调。set/open 返回值均未检查。当前先 open 再注册回调，应改为先完成回调注册再打开串口，以减少启用接收与安装处理器之间的时序风险。没有证据说明普通端口打开失败一定造成黑屏；本地 UARTBase::open 有返回 false 的失败分支。

下一次实机隔离应保留公共页注册，并且每一步只改变一个条件：

| 对照 | 结果的含义 |
|---|---|
| 只跳过 `open()`，保留 STTCom 创建、set 和回调注册 | 若恢复，优先检查 open 及其启动的接收事件；若仍失败，检查构造/set/生命周期 |
| 恢复 open，将回调体临时改为立即 return | 若恢复，故障在接收处理路径；若仍失败，检查 open、后台运行与公共页 |
| 恢复回调，仅将 `readData(bytes, 100)` 改成 `readData(bytes, 0)` | 若恢复，强烈支持等待事件循环重入问题；最终仍应确认日志与持续收包行为 |

跳过 open 的对照只能临时用于定位；全部注释公共页注册会禁用 HMI 的串口接收和生产记录，不能作为正式修复。定位时不要通过未授权的屏端下载自动执行这些试验。

辅助缺陷：wMCom3 未初始化为空指针，若构建失败后走 onDispose，其 `if (wMCom3)` 不能保证安全；应在构造函数初始化。异步 start() 失败时 dispatchPending 不复位属于收包停滞风险，但单独不足以解释整个系统启动失败。

补充证据：`MDK-ARM/MechanicalVerify/hmi_startup_audit/wcmn-runtime-disassembly.txt` 保存本次核查的注册、读取、回调和打开函数反汇编。以上为静态分析，没有修改 HMI 源码或替用户恢复公共页。

检查目标：`D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930/workspace/TEST`。

结论：本地源码能够完成 ARM 编译与链接。已找到两个有二进制证据的运行库兼容性风险，但没有屏端启动日志和实际部署文件，尚不能确定实机根因。本次只检查并生成验证产物，没有修改 HMI 源码、Release 文件或屏端程序。

## 1. 优先核对 libstt_core.so：同为 1.1.0.0，不代表二进制兼容

当前工程全部 `lib/` 头文件与安装目录的 `cn.simtutai.cdt_1.0.0.20250930` 模板逐文件一致，Release 链接路径也指向 20250930 的 ARM 运行库。

安装目录同时保留 202503171057 和 20250930 两套设备插件，均包含 `platform/1.1.0.0/arm/libstt_core.so`。对实际 `Release/libbootpanel.so` 的动态导入逐项检查：

| 运行库来源 | 大小 | 当前应用要求但运行库缺少的函数 |
|---|---:|---|
| 202503171057 / 1.1.0.0 | 2282116 字节 | `stt::STTWidget::enableDrag(bool)`、`stt::STTWidget::disBindData()` |
| 20250930 / 1.1.0.0 | 2445552 字节 | 未发现缺失的 STT 导入 |

缺失符号原名：

```text
_ZN3stt9STTWidget10enableDragEb
_ZN3stt9STTWidget11disBindDataEv
```

如果屏端实际加载的是上述旧库，当前应用无法正常完成动态加载，可能表现为黑屏、保留启动画面或由启动器反复重启。即使业务代码未直接调用这两个函数，派生控件的虚函数表也可能引用它们。

IDE 的 `configuration/org.eclipse.equinox.simpleconfigurator/bundles.info` 当前激活的是 20250930 插件。旧插件残留本身不证明 IDE 正在使用它，更不能证明屏端版本；必须核对实际部署的库。仅看到屏端版本号为 `1.1.0.0` 不足以排除此问题。

20250930 / 1.1.0.0 正确候选库的 SHA256：

```text
b43273d5210f7a7ca2a500e1cd43b8272f1e2f42c8bc5e272b9ca3c7b943c70b
```

建议：通过厂家支持的完整应用/运行环境更新流程，保证头文件、应用库、launcher 和 libstt_core 来自匹配版本。不要只覆盖应用 `.so` 后假定运行环境也已更新。不要为解决此问题直接改成 2.0.0；该库引入更高的 GLIBC 版本要求，需匹配屏端系统。

## 2. Release 与 C++11 验证构建的运行时依赖不一致

| 构建 | 大小 | CXXABI_1.3.9 依赖 |
|---|---:|---|
| 工程现有 Release/libbootpanel.so | 50452 字节 | 有：`_ZdlPvj`，即 sized operator delete |
| 当前源码显式 -std=c++11 重建 | 50048 字节 | 无 |
| 当前源码显式 -std=gnu++14 对照构建 | 50452 字节 | 有：同一符号 |

对照构建验证了编译语言标准可以引入这项额外依赖；对照库与现有 Release 的哈希不同，不能据此宣称已完全复现 IDE 的全部选项。

若屏端 `libstdc++.so.6` 不提供 `CXXABI_1.3.9`，现有 Release 会在加载时失败。当前没有屏端库，无法判断条件是否成立。建议在工控屏 Release 配置中明确使用 `-std=c++11`，清理并全量重建，避免验证脚本与实际下载构建采用不同的编译标准。

现有 Release 与重建库均要求 `GLIBCXX_3.4.21`、`GLIBC_2.17` 等。新增 `clock_gettime` 的确要求 GLIBC_2.17，但本地 1.1.0.0 框架本身已经依赖 GLIBC_2.17，所以不能仅凭这个调用认定它是此次黑屏根因。

## 3. 区分正常首页与真正的启动失败

工程默认页为 `Frm03`。其内容为“智能分拣”“R.C.C.O”“重启”“主菜单”，没有自动倒计时或启动后自动跳转逻辑。

如果屏幕已经显示这些内容且“主菜单”可点击，应用实际上已经启动，停留在此页是现有设计；点击“主菜单”进入 Frm02。若停留在厂家 Logo 或全黑，则继续检查加载器和初始化阶段。

`HmiApp::buildPage()` 对 Frm02 以外的页面请求返回 Frm03。检查本地框架初始化反汇编可见它会在公共窗口初始化后调用页面管理器打开页面；`onReady()` 为空本身不能作为“缺少首页启动代码”的证据，不建议盲目再加一次打开页面。

## 4. 较低优先级：公共串口初始化及收到数据后的 UI 工作量

`src/pages/Wcmn.cpp:27` 在公共窗口初始化阶段同步调用 `wMCom3->open()`，未处理其返回值；这发生在默认页面打开之前。如果屏端 COM2 驱动打开过程异常，这是需要加日志定位的启动检查点。目前没有证据证明该调用阻塞或崩溃。

无串口数据时，解析和列表重建逻辑不执行。若断开 MCU 到 HMI 的 RX 后仍然立即黑屏，应先排查库加载、资源与初始化，降低协议解析的优先级。

若无数据能启动、接入数据后卡住，则检查：

- `Wcmn.cpp:66` 的事件队列没有容量上限；异步任务 `start()` 的返回值没有处理，启动失败时 dispatchPending 会一直保留为 true。
- `Frm02.cpp:48` 每次同步先清空列表再逐条创建全部历史按钮，最多 256 条；一批多个事件会重复执行。这是持续数据/噪声下的 UI 卡顿与内存压力风险，不能解释完全没有输入时的加载失败。

这些是后续可靠性改进点，本次未将其修改为未经实机验证的替代实现。

## 5. 已完成的本地检查

- `python -X utf8 tools/build_hmi.py`：四个应用 CPP 完整编译、链接成功，输出未出现编译警告。
- ELF 检查：当前 Release 和重建结果均为 ELF32 / ARM / EABI5 hard-float / 共享库；AppMain、getAppArgs 导出存在，不是误把 Windows DLL 当 ARM 程序。
- 当前应用的 STT 动态导入在 20250930 / 1.1.0.0 核心库中均存在；这不等于完整屏端依赖加载测试。
- 字体 `fnt/wenquanyiweimihei.ttf` 存在，72 个动态引用的形状/颜色 PNG 均存在。
- 以只读方式执行本地 `data/default.db` 的 SQLite integrity_check，结果为 ok。
- 背光应用代码设置为 50，未发现应用初始化主动将背光置零。
- 现有 Release 的四个有效对象均在 2026-11-02 17:31 构建；残留 Frm01.o 未在当前库符号中发现对应页面代码，不能单凭旧对象文件存在认定链接了旧页面。
- IDE 日志尾部主要是另一个 MDemo 工程缺少 .project 的异常，不能用它证明 TEST 屏端崩溃原因。

本地资源完整不代表已下载到屏端；字体路径为相对路径，仍需核对部署内容及启动工作目录。

## 6. 实机验证顺序

1. 确认停留画面是否为 Frm03 正常首页，并尝试“主菜单”。
2. 取得一次失败启动的 launcher/系统日志，优先查 `undefined symbol`、`CXXABI_1.3.9 not found`、`GLIBCXX`、`cannot open shared object file`、`Segmentation fault`。
3. 核对屏端实际 libbootpanel.so、libstt_core.so、launcher 和 libstdc++.so.6。优先比较 libstt_core 的哈希与上表；如有 Linux 调试终端，可对实际应用路径执行 `ldd -r` 检查动态依赖和未解析符号。
4. 使用明确 C++11 的完整重建结果，按厂家支持流程部署匹配的 1.1.0.0 运行环境及字体/图片后验证。此操作尚未执行。
5. 若库加载正常仍失败，断开 HMI RX 对照启动；必要时依次在 AppMain、onInit、construct、Wcmn::createParts 的 open 前后、Frm03::createParts/onLoad 添加启动日志，确定最后完成的步骤。

## 证据文件

- `MDK-ARM/MechanicalVerify/hmi_startup_audit/audit.json`：库路径、哈希、缺失符号、资源及数据库检查结果。
- `MDK-ARM/MechanicalVerify/hmi_startup_audit/release-elf.txt`：实际 Release 的 ELF、依赖及符号信息。
- `MDK-ARM/MechanicalVerify/hmi_startup_audit/rebuilt-elf.txt`：C++11 重建结果的信息。
- `MDK-ARM/MechanicalVerify/hmi_startup_audit/gnu14/`：C++14 对照构建。
- `MDK-ARM/MechanicalVerify/hmi_arm/build.log`、`libbootpanel.so`：C++11 验证日志和候选库，未覆盖工程 Release，也未部署到屏端。
