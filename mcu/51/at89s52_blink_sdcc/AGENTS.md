# AGENTS.md（AT89S52 / 8051 分区）

本文件为 `at89s52_blink_sdcc/` 目录下的所有项目提供长期上下文入口，面向 AI Agent（或新同事）快速上手：知道代码放哪、怎么构建/烧录、以及常见的修改路径。

---

## 分区目标

- 以 **SDCC + CMake + Ninja** 为主线，建立 Windows Native 的 8051 开发工作流。
- 主要面向 AT89S52（经典 51 单片机）及兼容芯片（如 STC89C52）。
- 每个 `src/*.c` 文件可独立构建成一个 `.hex` 固件，便于切换功能。

---

## 关键目录与定位

```
at89s52_blink_sdcc/
├── CMakeLists.txt           # 主构建配置（定义多个固件目标）
├── main.c                   # 流水灯示例（主入口）
├── src/                     # 固件源码（当前仅保留已验证探针）
│   └── shift595_6digit_probe.c  # 6×74HC595 数码管探针
├── build/                   # 构建输出（*.ihx, *.hex）
├── AGENTS.md                # 本文件
└── GEMINI.md                # 安全协议（继承自全局 GEMINI.md）
```

---

## 默认工具链与产物

- **编译器**：SDCC (Small Device C Compiler)
  - Windows 安装路径：`C:\Program Files\SDCC\bin` 或 `C:\Program Files (x86)\SDCC\bin`
- **构建系统**：CMake + Ninja
- **产物**：
  - `*.ihx`（Intel HEX，SDCC 原生输出）
  - `*.hex`（标准 HEX，由 CMake post-build 复制生成）

---

## HEX 格式注意事项（重要）

- 部分 51 烧录软件会把 `packihx`“重新排版/重打包”后的 HEX 判定为“内容错误”（常见现象：同目录其它 `.hex` 能烧录，但某个新生成的不行）。
- 遇到这种情况：优先使用 **SDCC 原生输出的 `*.ihx`**，直接复制/改名为 `*.hex` 再导入烧录软件（不要再跑 `packihx`）。
- 快速命令示例：`sdcc ... --out-fmt-ihx -o build/ src/foo.c` 生成 `build/foo.ihx` 后执行 `Copy-Item build\\foo.ihx build\\foo.hex`。

---

## 构建/烧录（标准命令）

### 1) 配置（生成 build 目录）

```powershell
cmake -S . -B build -G Ninja
```

### 2) 构建所有目标

```powershell
cmake --build build
```

### 3) 烧录

使用 **STC-ISP** 或 **ProgISP** 等烧录软件手动加载 `build/*.hex`。
（51 单片机通常不支持在线调试，直接观察 LED/数码管现象即可）

---

## 常见修改路径（按目的找文件）

- 只改业务逻辑：`main.c` 或 `src/*.c`
- 添加新固件目标：在 `CMakeLists.txt` 中调用 `add_8051_firmware(target_name src/xxx.c)`
- 修改芯片参数（RAM/ROM 大小）：`CMakeLists.txt` 中的 `SDCC_FLAGS`
- 修改段码表（数码管）：`src/seven_segment.c` 或相关文件

---

## 硬件引脚说明（当前已知）

| 功能 | 引脚 | 备注 |
| :--- | :--- | :--- |
| 74HC595 串行数据 | P3.7 | `DS/SER`（示例代码里叫 `SI`） |
| 74HC595 移位时钟 | P3.6 | `SHCP/SRCLK`（示例代码里叫 `SCK`） |
| 74HC595 锁存时钟 | P3.5 | `STCP/RCLK`（示例代码里叫 `RCK`） |
| 启停按键（可选） | P2.7 | 通常按下为低电平 |
| LED（待测） | P1? | 需确认跳线帽 |

---

## 质量与验证（最低要求）

- 改动后至少做到：
  - CMake 配置成功（无 `Please specify...` 错误）
  - 构建成功，产出 `build/*.hex`
- 烧录后观察 LED/数码管现象是否符合预期。

---

## 常见问题（快速排查）

- **SDCC 报错 `unknown compiler option`**：忽略（SDCC 版本差异，不影响输出）。
- **构建成功但 .hex 没生成**：检查 CMakeLists.txt 中的 post-build copy 命令路径是否正确。
- **烧录后无现象**：检查跳线帽（LED_EN / SMG_EN），以及芯片是否正确插入。
- **数码管全亮 "8"**：可能是共阴极/共阳极配置反了，或扫描代码有问题。

---

## 变更边界（避免把工程弄乱）

- 不要随意删除 `build/` 以外的文件。
- 不要把敏感信息（密钥、序列号等）提交到任何文档或脚本。
- 保持每个 `.c` 文件的功能单一、可独立烧录。
