# AT89S52 Blink (SDCC)

一个最小可用的 AT89S52（8052 内核）示例：用 Timer0 做毫秒延时，周期性翻转 `P1.0`。

## 依赖

- `sdcc`（mcs51 后端）
- `packihx`（随 SDCC 一般会自带）

如果你用的是 Windows + Keil C51，也可以只把 `src/main.c` 迁过去编译（寄存器头文件不同需要调整）。

## 构建

```bash
make
```

生成文件在 `build/main.hex`。

## 烧录（USBasp / 专用 51 下载器）

你贴的 `usbipd list` 里 `VID:PID 16c0:05dc  USBasp` 说明你手上是 `USBasp`。

注意：`avrdude` 是 AVR（ATmega/ATtiny 等）用的工具，**不支持** `AT89S52`，所以 `make flash` 用 `avrdude` 会报 “Part not found” 属于正常现象。

推荐流程：

1) 在 WSL2 里只负责编译，生成 `build/main.hex`
2) 在 Windows 里用支持 `AT89S52` 的烧录软件（常见如 `ProgISP`/你板子配套的软件）选择芯片 `AT89S52`，导入 `build/main.hex` 烧录

## 参数配置

- 晶振频率：`FOSC_HZ`（默认 `11059200`）
- LED 逻辑：`LED_ACTIVE_LOW`（默认 `1`，表示低电平点亮）
- LED 引脚：`LED_PIN`（默认 `P1_0`，可用 `P1_1` / `P2_0` 等）

例如 12MHz 晶振：

```bash
make CPPFLAGS="-DFOSC_HZ=12000000UL"
```

例如把 LED 换到 `P1.1`：

```bash
make CPPFLAGS="-DLED_PIN=P1_1"
```

如果本机没有安装 SDCC（例如新装的 WSL2），先装它再 `make`：

```bash
sudo apt-get update
sudo apt-get install sdcc
```

（`avrdude` 不用于 AT89S52，可忽略。）

## 6 位数码管（6×74HC595 级联）连接说明

本仓库已验证的一种常见接法：用 3 根线驱动 **6 片 74HC595**，每片 595 的 `Q0..Q7` 直接驱动 **一位数码管的 8 段（含 dp）**，多片通过 `Q7'` 级联。

- 串行控制（来自 MCU `P3` 口）：
  - `P3.7` → `DS/SER`（示例代码里叫 `SI`）
  - `P3.6` → `SHCP/SRCLK`（示例代码里叫 `SCK`）
  - `P3.5` → `STCP/RCLK`（示例代码里叫 `RCK`）
- 级联：上一片 `Q7'` → 下一片 `DS`（一直串到第 6 片）
- 段码表：当前板子实测 **不需要取反**，可直接用常见的“共阴(1=亮)”段码（如 `0x3F/0x06/...`）
- 位序：当前板子实测显示顺序需要反转（即移位的字节落在“远端那片”），代码里用 `DIGIT_REVERSE=1` 处理
- 启停按键：`P3.2`（`INT0`）可用于启动/停止（默认按下为低电平）

已生成的相关固件（直接烧录 `.hex`）：
- 探针（自动切换顺序/取反模式，显示 `012345`）：`build/shift595_6digit_probe.hex`

额外示例固件（在本板已验证连线/位序前提下可用）：
- 秒表（从 `000000` 开始计时，按下停止并清零；再次按下从头开始；`P2.7` 按键）：`build/shift595_stopwatch_rev_btn_int0.hex`
- 秒表（同上逻辑，`P3.2/INT0` 按键）：`build/shift595_stopwatch_rev_btn_p32_int0.hex`

## 烧录提示（用你现有的 51 下载器）

你说“板子可以烧录”，通常下载器/软件只需要选择芯片型号 `AT89S52`，然后导入 `build/main.hex` 即可。

如果你是走 ISP（在板子上直接烧录），AT89S52 常见 ISP 信号是：

- `RST`（复位）
- `SCK`（P1.7）
- `MISO`（P1.6）
- `MOSI`（P1.5）
- `VCC` / `GND`

不同开发板可能把这些信号引到 6/10 针接口上，具体以板子丝印为准。

## WSL2 下 usbipd 转发（Windows → WSL）

如果你是用 **Windows 烧录软件**，USBasp 应该留在 Windows（不要 attach 到 WSL），否则 Windows 侧软件会找不到下载器。

如果你后续找到了 Linux 下能烧 `AT89S52+USBasp` 的工具（本仓库默认不提供），才需要把 USBasp 转发进 WSL。流程是：

1) Windows 管理员 PowerShell：

```powershell
usbipd bind --busid 2-1
usbipd attach --wsl --busid 2-1
```

2) WSL2 里确认：

```bash
lsusb
```

3) 需要断开时：

```powershell
usbipd detach --busid 2-1
```

STM32 的 ST-Link 也是同样流程：先把 ST-Link 插上电脑，再 `usbipd list` 找到设备名包含 `ST-LINK` / `STLink`（常见 VID:PID 是 `0483:3748` 等），对它的 `BUSID` 执行 `bind/attach`。
