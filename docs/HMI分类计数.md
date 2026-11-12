# HMI 分类页面同类总数

日期：2026-11-08。
目标工程：`D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930/workspace/TEST`。

## 显示与统计口径

分类记录行末追加 `同类总数:N`，表头图新增同名列。例如：

```text
ID:1  指令:73 正四面体-白  同类总数:4
ID:2  指令:80 球体-白  同类总数:2
```

- 以 code（完整的形状＋颜色）分别计数，覆盖原有72类、原8形状白色73..80及圆锥10色81..90，共90类。
- 相同 ID 下的不同形状/颜色分别计数；不同 ID 的相同 code 仍属于同类。
- 每收到一条合法 type=1 生产记录，该 code 的累计总数加1。同类的不同记录行显示该类当前总数，并随新记录同步刷新。
- 单字节预览、坏CRC/非法码帧、页面刷新和重开不增加总数。预览行不显示生产总数。
- 累计值独立于最多256条的可见历史；淘汰旧记录不减计数，不能用列表里某类的剩余行数代替累计总数。
- type=2 新生产会话同时清空列表、预览和90类计数；HMI重启也归零，不新增断电持久化或漏帧补算。

## 实现与部署

- `src/HmiApp.h`：90类 uint64_t 会话累计数组。
- `src/pages/Wcmn.cpp`：只在UI线程应用生产记录时增加计数，新会话清零。
- `src/pages/Frm02.cpp`：从独立计数数组读取当前总数，刷新历史行。
- `images/HDR-RECORDS.png`：增加同类总数表头，仍为971×41。
- 页面生成区及 `.vmf` 布局不改，改动保留在CUSTOM区；MCU协议、固件和运动流程不变。

ARM库和Windows模拟器DLL已编译，并同步至工程 `Release/libbootpanel.so` 和 `Debug/libbootpanel.dll`。
部署屏端时同时携带新库和更新的 `images/HDR-RECORDS.png`；尚未部署到实际屏幕。
ApusIDE 对工程 F5 刷新后 Clean / Build，避免继续使用外部修改前的对象文件。

## 验证

```powershell
python -X utf8 tools/selftest_hmi_worker.py
python -X utf8 tools/selftest_hmi_totals.py
python -X utf8 tools/build_hmi.py
python -X utf8 tools/make_hmi_header.py
```

- 真实 Wcmn 方法体：同形异色、同色异形、同ID多类、原形状白色8类、圆锥10色、连续300条后的历史淘汰、预览、会话清空、坏帧、dispose与worker启动失败均通过。
- 真实 Frm02 CUSTOM 方法：90类读取各自总数、同类多行更新到当前总数、重开不重复累计、预览不显示总数、空会话清屏、超过32位的计数字符串均通过。
- ARM C++11 编译/链接和Windows DLL构建通过，无编译警告。
- 表头已查看，字体测宽验证常规行及10位数量位于记录文本区域内。
- 本次模拟器未暴露可捕获窗口，未获得运行截图；实屏排版和实际串口显示仍待上机确认。

修改前快照：`MDK-ARM/MechanicalVerify/hmi_totals_backup/20261108-111723/`。
构建日志：`MDK-ARM/MechanicalVerify/hmi_arm/build.log`。
