# AGENTS.md（STM32 分区）

本文件为 `stm32/` 目录下的所有项目提供长期上下文入口，面向 AI Agent（或新同事）快速上手：知道代码放哪、怎么构建/烧录/调试、以及常见的修改路径。本文自包含关键信息与常用命令。

## 分区目标

- 以 **VS Code + CMake + OpenOCD + GDB** 为主线，建立“换芯片/换工程只改配置”的 STM32 学习与移植工作流。
- 主要面向 STM32F1（Blue Pill / STM32F103C8T6）与 STM32 标准外设库（StdPeriph）结构。
- 尽量让每个 `lesson_*` 工程可独立构建与调试，便于学习打卡与逐课迭代。

## 关键目录与定位

```
stm32/
├── learning/
│   ├── vscode_cmake_template/   # 通用模板（复制它来新建工程）
│   ├── lesson_*/                # 课程工程（每课一个可独立构建的固件工程）
│   ├── 课件/                    # 课程资料与外设库备份等
│   └── 进度打卡/                # 学习记录
└── docs/                        # STM32 相关文档（概念/资料索引等）
```

在每个具体工程目录（如 `learning/lesson_2-1/`）里，常见布局是：

```
<project>/
├── CMakeLists.txt
├── cmake/
│   ├── toolchains/arm-gcc.cmake      # ARM GCC 工具链文件
│   └── boards/<board>.cmake          # 板卡配置（芯片型号/链接脚本/启动文件/OpenOCD）
├── boards/<board>/                   # 板卡资源（openocd.cfg/链接脚本等）
├── drivers/                          # CMSIS + StdPeriph（或未来 HAL/LL）
├── include/                          # 工程头文件（如 stm32f10x_conf.h）
└── src/                              # 工程源码（main.c + 外设/组件）
```

## 默认板卡与产物

- 默认板卡配置名（CMake 变量 `BOARD`）：`stm32f103c8t6`
- 构建产物（在 `build/` 下）：
  - `firmware.elf`（调试与烧录推荐用它）
  - `firmware.bin`、`firmware.hex`（由 `objcopy` 自动生成）
  - `firmware.map`（链接映射与内存占用定位）

## 构建/烧录/调试（标准命令）

以下命令在任意工程目录内执行（例如 `stm32/learning/lesson_2-1/` 或 `stm32/learning/vscode_cmake_template/`）：

### 1) 配置（生成 build 目录）

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
  -DBOARD=stm32f103c8t6
```

### 2) 构建

```bash
cmake --build build -j$(nproc)
```

### 3) 烧录（推荐用工程自带 flash 目标）

板卡配置文件通常会设置 `OPENOCD_CFG`，从而自动生成 `flash` 目标：

```bash
cmake --build build --target flash
```

若需要手动调用 OpenOCD，工程里通常有 `boards/<board>/openocd.cfg` 可用。

## 常见修改路径（按目的找文件）

- 只改业务逻辑/演示功能：`src/`（优先改 `src/main.c`）
- 添加/修改外设驱动封装：`src/` + `include/`（保持模块边界清晰）
- 修改芯片型号/时钟/宏定义：`cmake/boards/<board>.cmake`（如 `BOARD_DEFINES`、`HSE_VALUE`）
- 修改 Flash/RAM 分配：`boards/<board>/*.ld`（链接脚本）
- 修改启动文件：`drivers/CMSIS/.../startup_*.s`（或在 board.cmake 里切换 STARTUP）
- OpenOCD/调试器参数：`boards/<board>/openocd.cfg`

## 新建一个 lesson（建议流程）

1. 从 `stm32/learning/vscode_cmake_template/` 复制出一个新目录（如 `lesson_XX-X/`）。
2. 只在新目录内改动：`src/`、必要时改 `cmake/boards/` 与 `boards/`。
3. 优先复用现有 `BOARD`（例如 `stm32f103c8t6`）；确实要换型号再新增 board 配置与链接脚本。
4. 先保证能 `cmake -S -B` 通过，再小步加入功能，避免一次性引入大量改动导致难排错。

## 质量与验证（最低要求）

- 改动后至少做到：
  - CMake 配置成功（不再报“必须指定工具链/板卡”的 FATAL_ERROR）
  - 构建成功，产出 `build/firmware.elf`
- 板卡/链接脚本/启动文件改动：建议额外做烧录验证（`--target flash`）并观察串口/LED 等最小可观测输出。

## 常见问题（快速排查）

- CMake 报错 “Please specify toolchain file”：配置命令缺少 `-DCMAKE_TOOLCHAIN_FILE=...`
- CMake 报错 “Please specify target board”：配置命令缺少 `-DBOARD=...`
- 烧录后无反应：优先检查 BOOT0 跳线（应接 GND）、供电、复位电路与下载线连接
- 串口无输出：确认波特率、引脚复用与连线（RX/TX 交叉）
- OpenOCD 连接失败（Windows Native）：检查 ST-Link 驱动是否被替换为 WinUSB（使用 Zadig 工具），以及 OpenOCD 脚本路径是否正确。

## 变更边界（避免把工程弄乱）

- 不要为了“更优雅”随意重构模板结构；学习工程优先稳定、可复制、可复现。
- 不要把大量代码风格规范塞进文档；格式化/静态检查应交给确定性工具或 CI/钩子。
- 不要提交敏感信息（密钥、序列号、公司内网地址等）到任何文档或脚本。

