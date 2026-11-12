# TEST (bootpanel) — ApusIDE HMI 应用

基于 SimTutai ApusIDE v2.08 的嵌入式 HMI 应用，用于垃圾分拣系统的串口通信与图形界面。

## 项目

- **堆栈**: C++11 → ARM Linux 共享库 (libbootpanel.so)
- **框架**: ApusIDE (SimTutai/芯途智联)，核心库 libstt_core
- **入口**: `src/HmiApp.cpp` → `AppMain()` 返回 `new HmiApp(env)`
- **功能**: 通过串口接收下位机数据，显示垃圾分拣状态（有害/可回收/厨余/其他垃圾）

## 命令

```bash
# 交叉编译（在 ApusIDE 内通过 Eclipse CDT 管理构建）
# 工具链: gcc-linaro-6.3.1 arm-linux-gnueabihf-g++
# 输出: libbootpanel.so → 部署到 ARM 设备

# 手动编译示例（参考 reasonix.toml 中的构建记录）：
CXX="D:/BaiduNetdiskDownload/ApusIDE2_v208/ApusIDE2_v207/sdk/gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++"
$CXX -c -O3 -fPIC -std=c++11 -I"$PROJ/lib" "$PROJ/src/HmiApp.cpp" -o HmiApp.o
$CXX -shared -O3 *.o -lstt_core -lpthread -ldl -lstdc++ -o libbootpanel.so
```

## 架构

```
src/
├── HmiApp.h / HmiApp.cpp    — 应用入口类，继承 HmiAppBase；construct() 注册变量/字体/公共窗口
└── pages/
    ├── Frm02.h / Frm02.cpp  — 页面类（继承 STTPage），createParts() 构建 Widget 树
    ├── Frm03.h / Frm03.cpp  — 页面类
    └── Wcmn.h / Wcmn.cpp    — 公共命令窗口，处理串口(STTCom)收发与业务逻辑
lib/                         — 框架头文件（只读，不修改）
├── base/        — Interface.h (IRuntime/IApplication), Constants.h
├── core/        — SttObject, SttSignal, SttTask, SttLooper 等运行时核心
├── type/        — 数据类型: Tint, Tfloat, Tvchar, TTable 及对应的 D* 数据绑定类
├── widgets/     — UI 控件: STTPage, STTButton, STTCom, STTLabel, STTChart, STTMeter...
├── op/          — 操作层: EventMgr, DataSample, RCP, OpLog
├── db/          — 数据库接口: IDataBase
├── misc/        — Printer, misc 工具
└── util/        — DateUtil, FileUtil, StringUtil, Util
pages/          — .vmf 页面布局描述文件（IDE 生成，XML 格式）
data/           — default.db 本地数据库
fnt/            — 字体文件 (wenquanyiweimihei.ttf 等)
```

**核心概念**:
- `vm` (变量管理器): `vm->add(name, type, json)` 注册变量，`vm->getInt/setInt/getChar/setChar` 读写
- `pm` (页面管理器): `pm->open(id, mode)` 打开页面，页面模式 NEWWIN / EMBEDDED
- `fm` (字体管理器): `fm->registerFontFile(name, path)`
- 页面生命周期: `onInit()` → `createParts()` → `onLoad()` → ... → `onClosing()` → `onDispose()`

## 约定

- **文件头注释**: 每个源文件有块注释标明用途和"用户手动修改"/"IDE自动生成，用户勿修改"标记
- **标记区域**: `/*CUSTOM_CODE_BEGIN*/` / `/*CUSTOM_CODE_END*/` 和 `/*CUSTOM_HEAD_BEGIN*/` / `/*CUSTOM_HEAD_END*/` 之间的代码由用户编写，外部区域由 IDE 管理
- **include 防护**: 使用 `#ifndef PATH_TO_FILE_H` / `#define PATH_TO_FILE_H` 模式（如 `SRC_PAGES_FRM02_H`）
- **命名空间**: 统一 `using namespace stt;`，子命名空间按需引入 (`stt::op`, `stt::util` 等)
- **页面模式**: 每个页面类继承 `STTPage`，持有 `HmiApp* hmiApp` 指针，构造函数接收 `(HmiApp*, STTWidget*, id, desc)`
- **Widget 创建**: 在 `createParts()` 中用 `new` 创建 Widget，设置属性后由框架管理生命周期
- **串口**: 使用 `STTCom` Widget，通过 JSON 配置串口参数，绑定 `VALUE_CHANGED` 事件回调
- **C++标准**: C++11，不使用更高版本特性
- **lib/ 目录只读**: 不修改 lib/ 下的框架头文件

## 备注

- 项目通过 ApusIDE（Eclipse CDT 定制版）管理，构建和部署由 IDE 控制
- 目标平台为 ARM Linux（arm-linux-gnueabihf），非 x86 Windows
- `reasonix.toml` 中包含历史构建命令参考
