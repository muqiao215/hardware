> **第三方内容说明（2026-09-13 迁入时注明）**
>
> 本目录由原仓库 `Learn_stm32f103`（Embedded Lab Monorepo）迁入，**仅保留本人原创部分**：`rpi_hpad`（树莓派 HPAD 采集板工具链）、`WM8960-Audio-HAT`、`pi_gcc_f103_blink`、`mcs51` 迁移验证链，以及 `common/`、`boards/`、`cmake/`、`scripts/`、`tests/`、`plans/`、`examples/` 等构建与验证设施。
>
> 原仓库曾包含以下**第三方内容，已全部移除**，特此注明来源与作者：
> - 各章节笔记（`00stm32编程技巧.md`、`01STM32简介.md`、`02软件安装~11SPI` 章节目录及 .rar 资源包）：作者 **jjejdhhd**，源自 https://github.com/jjejdhhd/Learn_stm32f103
> - `mcs51/Learn_STC89C52/`：导入自 https://github.com/jjejdhhd/Learn_STC89C52（详见 `mcs51/UPSTREAM.md`）

# Embedded Lab Monorepo

这是一个面向学习、迁移、验证和长期维护的嵌入式 monorepo。

它现在包含三条独立主线：

- `STM32F103` 裸机示例与平台层
- `STC89C52 / MCS-51` 迁移与最小验证链
- `Raspberry Pi HPAD` 采集板软件工具链
- `WM8960 Audio HAT` 树莓派音频编解码板驱动与安装资源

当前目标不是继续堆更多例程，而是把三条线都收成：

- 可构建
- 可验证
- 可交接

## 一眼看懂

当前冻结状态：

- `stm32_f103`: `v2.0.0-soft-validated`
- `mcs51`: 软件侧破冰完成，真板验证 pending
- `rpi_hpad`: `v0.1.0-soft-validated`
- `wm8960_audio_hat`: vendor-imported, hardware-present

硬件总状态：

- STM32F103: `hardware_pending`，当前未检测到 ST-Link
- STC89C52: `hardware_pending`，等待真实板级串口/按键验证
- Raspberry Pi HPAD: `hardware_pending`，`real` backend 仅保留配置边界，未声明 ADS1256/DAC8532 已验证
- WM8960 Audio HAT: 当前实机已识别到 `wm8960-soundcard`，仓库内保留 vendor 驱动/overlay/安装资源

如果你只想快速上手，先看这里：

```bash
scripts/check-all.sh
```

它会顺序运行三条线的无硬件 gate，确认当前仓库的软件面仍然成立。

## 仓库地图

```text
.
├── boards/ common/ examples/ cmake/ scripts/   # STM32F103 主线
├── mcs51/                                      # STC89C52 主线
├── rpi_hpad/                                   # Raspberry Pi HPAD 主线
├── WM8960-Audio-HAT/                           # WM8960 Audio HAT 主线
├── tests/                                      # STM32 host-side tests
└── plans/                                      # 迁移与收口过程文档
```

## 子项目

### STM32F103

路径：仓库根目录的 `examples/`, `common/`, `boards/`, `cmake/`, `scripts/`

定位：

- `STM32F103C8T6 / Blue Pill`
- `GNU Arm Embedded + CMake + Ninja`
- 裸机示例、模块化平台层、无板子构建 gate、验证包生成

主入口：

```bash
scripts/check.sh
scripts/build-all.sh
scripts/probe-stlink.sh
scripts/flash.sh 01_gpio_led
```

适合你现在做的事：

- 看平台层和示例如何解耦
- 跑 `host-side tests`
- 打验证包，等真板接手代测

已迁移示例：

- `01_gpio_led`
- `02_oled_i2c`
- `03_exti_key`
- `04_tim_pwm`
- `05_adc_polling`
- `06_adc_dma`
- `07_usart_cli`
- `08_i2c_hw`
- `09_spi_loopback`

Legacy 课程资料仍保留在：

- `02软件安装及创建工程/`
- `03GPIO通用输入输出口/`
- `04OLED调试工具/`
- `05EXIT外部中断/`
- `06TIM定时器/`
- `07ADC模数转换器/`
- `08DMA直接存储器读取/`
- `09USART串口/`
- `10IIC通信/`
- `11SPI通信/`

### MCS-51 / STC89C52

路径：`mcs51/`

定位：

- STC89C52RC
- `SDCC + packihx + stcgal`
- 薄适配层、最小构建链、串口观察口、可主机测试的 debounce 逻辑

主入口：

```bash
cd mcs51
scripts/check.sh
scripts/probe-toolchain.sh
scripts/build-all.sh
```

适合你现在做的事：

- 看 `SDCC` 最小构建链
- 看 `uart_tx` 和 `debounce` 如何从课程代码里剥出来
- 等真板后先验证串口和按键，再考虑 LCD

当前示例：

- `01_blink`
- `03_uart_tx`
- `04_key_debounce`

硬件边界：

- 真实串口输出、按键脚位、烧录链路仍需真板验证
- 查看 `mcs51/HARDWARE_PENDING.md`

### Raspberry Pi High-Precision AD/DA Board

路径：`rpi_hpad/`

定位：

- Raspberry Pi 侧 High-Precision AD/DA Board 软件工具链
- Python package
- `mock / replay / real` backend 模型
- `real` backend 当前只保留配置和诚实错误边界

主入口：

```bash
cd rpi_hpad
scripts/check.sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -e .
hpad --help
```

适合你现在做的事：

- 先用 `mock -> log -> replay` 跑完整软件链
- 把它当一个 Python 工具包来用
- 等真板到了再补 `real` backend 的 SPI/GPIO 接入

当前冻结：

- `rpi_hpad_event_v1` JSON event protocol
- `mock -> log -> replay` 软件闭环
- `real_backend_not_implemented` 稳定错误码

文档：

- `rpi_hpad/QUICKSTART.md`
- `rpi_hpad/PROTOCOL.md`
- `rpi_hpad/RELEASE_NOTES.md`
- `rpi_hpad/CHANGELOG.md`

### WM8960 Audio HAT

路径：`WM8960-Audio-HAT/`

定位：

- Raspberry Pi 侧 `WM8960` 音频编解码小板
- vendor 驱动、overlay、DKMS 和 ALSA 配置资源
- 面向耳机/喇叭播放与录音输入链路

主入口：

```bash
cd WM8960-Audio-HAT
sed -n '1,200p' README.md
```

适合你现在做的事：

- 对照现成驱动和 overlay 看板卡接线
- 检查 `wm8960-soundcard.dts` 和 `asound.conf`
- 在树莓派侧保留一份可追溯的音频模块来源

当前边界：

- 当前目录是 vendor-imported 资源，不是统一成仓库自研风格的新子系统
- 安装脚本会修改系统，使用前应先审查

文档：

- `WM8960-Audio-HAT/README.md`
- `WM8960-Audio-HAT/wm8960-soundcard.dts`
- `WM8960-Audio-HAT/asound.conf`

## 总检查

运行三条线的无硬件 gate：

```bash
scripts/check-all.sh
```

该脚本会依次执行：

- STM32F103: `scripts/check.sh`
- MCS-51: `mcs51/scripts/check.sh`
- RPi HPAD: `rpi_hpad/scripts/check.sh`

## 推荐入口

如果你是第一次看这个仓库，建议按这个顺序：

1. 先读本页，确认三条线的边界
2. 跑 `scripts/check-all.sh`
3. 按兴趣进入 `STM32F103`、`mcs51/`、`rpi_hpad/` 或 `WM8960-Audio-HAT/`
4. 真板相关事项只看各自的 `HARDWARE_PENDING.md` / `REALBOARD_VALIDATION_CHECKLIST.md` 或 vendor README

## 环境依赖

STM32F103：

```bash
sudo apt install cmake ninja-build gcc-arm-none-eabi binutils-arm-none-eabi stlink-tools
```

MCS-51：

```bash
sudo apt install sdcc
python3 -m pip install --user stcgal
```

RPi HPAD：

```bash
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -e rpi_hpad
```

WM8960 Audio HAT：

```bash
cd WM8960-Audio-HAT
sed -n '1,200p' README.md
```

## 结构原则

- 三条线共用一个 git 仓库，但不共用一个构建系统
- 四条线共用一个 git 仓库，但不共用一个构建系统
- 每条线保留自己的 README、脚本、硬件 pending 文档和验证 gate 或 vendor 文档
- 根目录只提供总入口和总检查，不把 STM32、51、树莓派采集板、音频板强行揉成一个工程
- legacy 资料保留，不作为当前主动开发入口

## 当前不宣称

- 不宣称 STM32 已完成真板烧录和运行观察
- 不宣称 STC89C52 已完成真板烧录、串口和按键验证
- 不宣称 ADS1256/DAC8532 已完成 SPI/GPIO/精度验证

所有硬件结果必须等真实板子连接后，以对应子项目的验证清单为准。
