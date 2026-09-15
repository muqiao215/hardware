# 🛑 GEMINI.md（AT89S52 分区安全协议）

本文件继承自全局 `GEMINI.md`，并针对 8051/SDCC 开发环境补充特定规则。

---

## 🔗 继承规则（来自全局 GEMINI.md）

1.  **环境变量安全**：绝对禁止将 `$env:Path` 直接写入 User 变量。必须使用 `[Environment]::GetEnvironmentVariable("Path", "User")` 读取原值后追加。
2.  **编码安全**：在执行可能产生非 ASCII 输出的命令前，强制设置 `[Console]::OutputEncoding = [System.Text.Encoding]::UTF8`。
3.  **文件操作确认**：高风险操作（如 `Remove-Item -Recurse`）需谨慎。

---

## 🛡️ 8051/SDCC 特定规则

### 1. SDCC 关键字差异
- SDCC 使用 `__code`, `__data`, `__xdata` 等关键字，**不同于 Keil 的 `code`, `data`, `xdata`**。
- 从 Keil 示例代码移植时，必须手动转换关键字，否则编译报错。

### 2. CMake 缓存污染
- 修改 `CMakeLists.txt` 中的 `SDCC_FLAGS` 或添加新目标后，**必须删除 `build/` 目录**后重新配置。
- 否则 CMake 会使用缓存的旧配置，导致不可预期的行为。

### 3. 烧录器兼容性
- STC 系列芯片（如 STC89C52）使用 **STC-ISP**，需要冷启动（断电再上电）触发烧录。
- AT89S52 使用 **ISP 编程器**（如 USBasp），无需冷启动。

---

## 📝 项目特定记录

### 2026-01-15 数码管引脚确认
- 六位八段数码管：
  - **段选**：P0
  - **位选**：P3.5, P3.6, P3.7（通过 74HC138 译码）
- 屏幕类型：**共阴极 (Common Cathode)**

---

*Last Updated: 2026-01-15 by Antigravity/Qingmeng*
