# ApusIDE（芯途智联）HMI 屏调试手册

日期：2026-11-03。适用工程：`D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930/workspace/TEST`（`libstt_core` / `libbootpanel.so` 闭源 SDK）。
本文是 2026-11-02 ~ 11-03 三轮实机/模拟器排查的方法沉淀：黑屏、串口记录不显示、列表首行无文字、IDE 改了不生效。

## 0. 三条铁律

1. **先确认跑的是哪份二进制**。屏端/Debug 目录里的 `.so`/`.dll` 可能不含本次改动（IDE 增量构建复用旧 `.o` 是常态）。用 `strings -a <产物> | grep -c <改动里的唯一字符串>` 验证，别靠时间戳猜。
2. **SDK 行为一律用反汇编和最小复现证明**，不引用文档措辞猜测（文档与实际派发/事件码常不一致，例如 `STTTimer` 的事件码）。
3. 每轮改动固定顺序：**备份 → 主机 mock 单测 → ARM 构建 → 模拟器复现/验证 → 写回文档**。

## 1. 环境地图

```bash
SDK=/d/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930
PROJ=$SDK/workspace/TEST                 # HMI 工程（交付目标）
ARM=$SDK/sdk/gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf/bin
WIN=$SDK/sdk/mingw64/bin                 # g++ 8.1.0，模拟器 DLL 用它
DEV=$SDK/plugins/cn.simtutai.device.simtutai_1.0.0.20250930/bundle   # exec/ lib/ platform/
L3=$SDK/plugins/cn.simtutai.3rdlib_1.0.0.202502170859/bundle/dll/win # SDL2/freetype/jsonc/sqlite3
```

工程结构：`src/HmiApp.cpp`（`construct()` 里 `vm->add`、`fm->registerFontFile`、`regCmdWindow`）、`src/pages/FrmNN.cpp`（`createParts` 为 IDE 生成区，**勿改**；逻辑放 `/*CUSTOM_CODE_BEGIN*/` 内）、`pages/*.vmf`（页面 JSON）、`lib/`（SDK 头，只读）、`Debug/`（win 模拟器产物）、`Release/`（ARM 产物）、`images/ fnt/ data/ res/`。

**公共页**（`regCmdWindow(cWnd, new Wcmn(...))`）的 `createParts`/`onLoad` 在**启动阶段**就执行，不要假设它只在“打开某页”时才跑——模拟器里加 `fprintf` 即可证实（实测启动即打印）。

## 2. 三件套验证

### 2.1 主机 mock 单测（最快，先做）

真编译 `Wcmn.cpp` 本体，只替换 SDK 头：脚本剥掉 `^#include` 与 `using namespace stt` 行，前置一个 mock 头。模板：`tools/selftest_hmi_worker.py` + `tests/mechanical/hmi_worker_mock.h`。

mock 的价值在于**用断言固化线程契约**：

```cpp
// UI 侧 mock 自己断言必须运行在 onFinished 里
void syncCommandHistory() { assert(MockWorkerState::inUi()); ++refreshes; }
void setChar(const string&, const string& s, int) { assert(MockWorkerState::inUi()); last = s; }
// 串口侧 mock 断言不允许等待、不允许在 UI 回调里读
bool readData(vector<uint8_t>& b, int timeout) { assert(timeout == 0); assert(!MockWorkerState::inUi()); /*...*/ }
// AsynWorker::start() 只把 onFinished 推队列，测试用 RunPendingUi() 驱动
```

构建：`g++ -static -std=c++11 -Wall -Wextra -I<生成头目录> -I<工程src> Wcmn.cpp test.cpp -o test.exe`。
覆盖点：未 `onLoad` 不读串口、零等待、分包/200 ms 超时、CRC 拒绝、一批只跳一次 UI、历史上限、缺页打开、dispose 丢弃、异步 `start()` 失败不越权刷 UI。

### 2.2 ARM 交叉构建

`python -X utf8 tools/build_hmi.py` → `MDK-ARM/MechanicalVerify/hmi_arm/libbootpanel.so`。要求 `ERROR_COUNT: 0`、`WARNING_COUNT: 0`，再按 5.1 校验导入符号。

### 2.3 Windows 模拟器（能跑能截图，性价比最高）

组装 run 目录：

| 来源 | 内容 |
|---|---|
| `$DEV/exec/1.1.0.0/win/` | `launcher.exe` |
| `$DEV/platform/1.1.0.0/win/` | `libstt_core.dll`、`libstt_lvgl.dll`、`libstt_assets.dll` |
| `$DEV/lib/1.1.0.0/win/` | `SDL2_mixer.dll`、`libcurl/ssl/crypto` |
| `$L3/` | `SDL2.dll`、`libfreetype-6.dll`、`libjsonc.dll`、`sqlite3.dll` |
| `$WIN` | `libstdc++-6.dll`、`libgcc_s_seh-1.dll`、`libwinpthread-1.dll` |
| 工程 | `pages/ images/ fnt/ data/ res/` + 编出的 `libbootpanel.dll` |

```bash
launcher.exe -w 1024 -h 600 -z 50 start    # z=缩放；窗口标题为 SIMULATOR
```

编译（与 IDE Debug 配置等价，`-g -O0`）：

```bash
g++ -c -g -O0 -std=c++11 -I<proj>/lib <各 src/*.cpp> -o <obj>
g++ -shared -g -O0 <objs> -L<platform/win> -lmingw32 <platform/win>/libstt_core.dll -lws2_32 -lpthread -lstdc++ -o libbootpanel.dll
```

**截图**（SDL/D3D 也可靠）：按 PID 枚举窗口 → `PrintWindow(hwnd, hdc, PW_RENDERFULLCONTENT=2)` → `GetDIBits` → `Image.frombuffer("RGB", ..., "BGRX")`。脚本：`MDK-ARM/MechanicalVerify/hmi_win_run/simshot.py`（单帧）与 `simshot2.py`（多时刻，如 `4.5,9.5`）。截图前必须 `SetWindowPos(HWND_TOPMOST, SWP_SHOWWINDOW)`，否则被其它窗口遮挡。

**注入真实数据**：把工程复制成 scratch 副本（**绝不改交付目录**），只在 `onLoad()` 里起线程，按时间点调业务入口并打日志：

```cpp
std::thread([this]() {
  sleepMs(2500);
  vector<uint8_t> bytes = parse_hex("F0 80 81 80 81 80 81 80 80 80 80 80 80 81 83 80 83 86 8F 8E 8D F1");
  consumeSerial(bytes);              // 与真实接收线程同一入口
  fprintf(stderr, "[SIM] injected %zu bytes\n", bytes.size()); fflush(stderr);
}).detach();
```

`launcher.exe` 是控制台程序，`stderr` 重定向到文件即可拿日志。

**像素级断言**（比肉眼看截图可靠）：对固定区域统计非白/暗像素数量、采样文字颜色，用于判断“白底白字”“列是否对齐”。例：`sum(1 for px in region.getdata() if px != (255,255,255))`。

## 3. 串口协议自测

MCU↔屏链路：115200 8N1，两种编码——**旧单字节预览** `1..90`（原1..72保持原表，原8形状白色73..80、圆锥10色81..90；见 圆锥系列几何体映射.md）；**新 22 字节帧** `F0` + 10 字节拆成 20 个 `0x80|nibble` + `F1`，CRC16/CCITT-FALSE（init 0xFFFF、poly 0x1021、低字节先发），`data[0]=version=1`、`data[1]=type`、`data[2..5]=seq(LE)`、`data[6]=code`、`data[7]=id`。

已用真实解析器核对、可直接发的三帧：

```text
新会话   F0 80 81 80 82 80 80 80 80 80 80 80 80 80 80 80 80 80 8E 81 86 F1
记录19/3 F0 80 81 80 81 80 81 80 80 80 80 80 80 81 83 80 83 86 8F 8E 8D F1
预览19   13
```

- 生成/发送：`python -X utf8 tools/hmi_frame_test.py [--port COM7] [--reset] [--record 19 3] [--preview 19]`；
- **自造帧必须回灌给真解析器验证**，别信手算 CRC：写个小 C++ `#include "HmiRecord.h"`，逐字节 `parser.feed()`，坏 CRC 必须 0 事件；
- 会被静默丢弃：CRC 错、半字节不在 `0x80..0x8F`、长度≠20、`version≠1`、type=2 时 seq/code/id 非 0、type=1 时 code∉1..90 或 id∉1..6、分包间距 > 200 ms；
- 串口助手必须 **HEX 模式**、一帧连续发出、不要带回车换行。

## 4. 症状 → 动作

| 症状 | 先查 |
|---|---|
| 黑屏 / 停启动画面 | 产物是否同一构建；`nm -D` 查缺失导入（5.1）；`readelf -V` 查 `CXXABI_1.3.9`/`GLIBCXX` 是否高于屏端；launcher 日志有无 `undefined symbol`/`Segmentation fault` |
| 行为与源码不符 | `strings` 标记 + `src/**.cpp` vs `Debug/src/*.o` vs 产物 mtime 三者对齐（6.1） |
| 列表首行只有图没文字 | SDK 列表首行丢样式（5.2），用表头图占位 |
| 串口有数据但界面不动 | 回调事件码/线程模型（5.2）；是否在回调里直接改 UI；是否 `readData` 正超时 |
| 单条记录正常、多帧丢 | 批处理语义、解析超时、UI 一次跳转；用主机单测复现 |
| 定时器回调从不执行 | `STTTimer::onEventHandler` 只认 code 31 |

## 5. SDK 行为核查手法与已确认坑

### 5.1 动态依赖与符号

```bash
UND=$($ARM/arm-linux-gnueabihf-nm.exe -D --undefined-only app.so | awk '{print $2}' | sort -u)
DEF=$($ARM/arm-linux-gnueabihf-nm.exe -D --defined-only libstt_core.so | awk '{print $3}' | sort -u)
comm -23 <(echo "$UND") <(echo "$DEF") | grep '^_ZN3stt'    # 屏端核心库必须全覆盖
$ARM/arm-linux-gnueabihf-readelf.exe -V app.so              # 版本需求
```

安装目录常同时存在多个插件版本（`202503171057` 与 `20250930`），**同名 `1.1.0.0` 不等于二进制兼容**；屏端固件版本号相同也不能排除。

### 5.2 反汇编查语义（本项目已确认）

```bash
$ARM/arm-linux-gnueabihf-objdump.exe -T libstt_core.so | grep 'STT'
$ARM/arm-linux-gnueabihf-objdump.exe -d --start-address=0x... --stop-address=0x... libstt_core.so
```

- **`STTTimer::onEventHandler` 只接受 `code == 31`（`Event::READY`）**，其它事件码直接 return（汇编 `cmp r1,#31; bne`）；IDE 生成代码也用 `Event::READY`。注册 `VALUE_CHANGED`(28) = 静默失效、回调永不执行。
- **`STTCom::onEventHandler` 不校验 code**，但派发恒传 31；回调运行在 COM 接收线程（`new STTCom(..., true)`）。结论：回调里可读+解析，**不能碰 `vm/pm/控件`**。
- **`STTCom::readData(vec, wait>0)` 会在等待期调用 `SttLooper::currentLooper()->exec()`**，在接收线程里等于重入 SDK 事件循环；回调里必须 `readData(vec, 0)`。
- 跨线程回 UI 的官方路径是 `IAsynWorker`（头文件写明：`invoke`=禁止 UI 操作，`onFinished`=可 UI 操作）。要么事件驱动跳一次 UI，要么 UI 线程 `STTTimer` 轮询；此外没有“在串口回调里直接刷界面”的安全路径。
- **`STTList` 在 `clearOptions()` 之后创建的第一个子行不会套用列表文字样式**（白字，白底等于看不见），第二行起正常。已逐项排除：`display()`、`refreshAll()`、`refresh()`、重建后再 `setFonts`、`setMainStyle` 抖动、`setSelected(-1)`、`p_text->setColor()`、二次清空重建、下一轮 UI 重建——全部无效。可行规避：首行放**只带图片的表头行**（文字画进 PNG），真实行从第二行起正常。
- 公共页 `createParts`/`onLoad` 随应用启动执行；`onLoad` 里创建的定时器会被启动流程直接命中。

## 6. IDE（Eclipse CDT）协作

### 6.1 “改了没生效”最常见的原因

Eclipse 不感知**IDE 外部**对源文件的修改，增量构建复用旧 `.o` 再链接，产物里没有新代码。判定：

```bash
stat -c '%y %n' src/pages/FrmNN.cpp Debug/src/pages/FrmNN.o Debug/libbootpanel.dll
strings -a Debug/libbootpanel.dll | grep -c <改动里的唯一字符串>   # 0 = 没进去
```

修复：工程上 **F5 刷新 → Project → Clean… → Build**；或关掉 IDE 删 `Debug/` 重建。长期：`Preferences → General → Workspace → Refresh using native hooks or polling` 打勾。

### 6.2 页面重生成

在 IDE 里打开/保存页面会重写该页 `.cpp/.h`：`createParts` 等生成区被覆盖，`/*CUSTOM_CODE_BEGIN*/…/*CUSTOM_CODE_END*/` 内保留。**所有逻辑都放 CUSTOM 区**：需要改列表字体颜色这类“生成区里的样式”，也改成在 CUSTOM 函数里调用，不要去动 `createParts`。

### 6.3 断点调试

`workspace/.metadata/.log` 里的 `org.eclipse.cdt.dsf.gdb` 空指针（“Context cannot be suspended”）是 CDT 附加到 `launcher.exe` 载入的 DLL 时的问题。实用替代：模拟器里 `fprintf(stderr, ...)` + `fflush` 重定向到文件；或在 Debug 配置里用 GDB attach 到 launcher 进程。

## 7. 交付与记录习惯

- 改动前备份到 `MDK-ARM/MechanicalVerify/<feature>_backup/<YYYYmmdd-HHMMSS>/`；
- 文档同步：工程内设计说明 + 本仓库 `docs/`（记录假设、证据、已排除项，别只写结论）；
- 中文源文件为 **CRLF、UTF-8 无 BOM**：改完核对行尾（`read().count(b'\r\n')`），避免整文件 LF 造成全量 diff；
- 屏端部署清单要包含**新增资源**（如 `images/HDR-RECORDS.png`），只拷 `.so` 会出现空白表头/缺图；
- 交付库路径：`MDK-ARM/MechanicalVerify/hmi_arm/libbootpanel.so`，覆盖工程 `Release/` 前先确认构建参数与屏端运行库匹配。

## 8. 可复用脚本

| 脚本 | 用途 |
|---|---|
| `tools/build_hmi.py` | ARM 交叉构建 + ELF 头输出 |
| `tools/selftest_hmi_worker.py` + `tests/mechanical/hmi_worker_mock.h` | 真实方法体的线程边界/协议单测 |
| `tools/hmi_frame_test.py` | 生成/发送 22 字节帧与单字节预览 |
| `tools/make_hmi_header.py` | 列表表头图（按列对齐、项目字体）生成 |
| `MDK-ARM/MechanicalVerify/hmi_worker_sim2/build_win.py` | 工程 → 模拟器 DLL |
| `MDK-ARM/MechanicalVerify/hmi_win_run/simshot.py`、`simshot2.py` | 启动模拟器并截图 |
| `MDK-ARM/MechanicalVerify/hmi_win_run/probe_fix.py` | 多种规避方案批量跑 + 像素判定 |

## 9. 快速命令卡

```bash
# 1 主机单测
python -X utf8 tools/selftest_hmi_worker.py
# 2 ARM 构建 + 符号校验
python -X utf8 tools/build_hmi.py
# 3 模拟器：编 DLL → 装配 run 目录 → 截图
python MDK-ARM/MechanicalVerify/hmi_worker_sim2/build_win.py "$PROJ"
cd MDK-ARM/MechanicalVerify/hmi_win_run && python simshot.py . shot.png 10
# 4 发帧
python -X utf8 tools/hmi_frame_test.py --port COM7 --reset --record 19 3
# 5 判定产物
strings -a Debug/libbootpanel.dll | grep -c <唯一标记>
```

## 10. 证据来源

- `MDK-ARM/MechanicalVerify/hmi_startup_audit/`：`STTTimer` 构造与 `STTCom::readData`/`onDataReceiveHandler`/`regCmdWindow` 反汇编；
- `MDK-ARM/MechanicalVerify/hmi_worker_test/`：主机单测产物；
- `MDK-ARM/MechanicalVerify/hmi_win_run/`：模拟器 run 目录、截图与探测脚本；
- `MDK-ARM/MechanicalVerify/hmi_polling_backup/`：各轮改动前备份；
- 指定工程内 `通信与分类显示说明.md`：协议与实现细节；`docs/HMI启动软件排查.md`：黑屏排查过程。