# MCS-51 Hardware Pending

当前 51 线的软件侧已经走到可构建、可主机测试、可交接状态，但真板验证仍未完成。

当前状态：

- `01_blink`：软件构建已验证
- `03_uart_tx`：软件构建已验证
- `04_key_debounce`：软件构建与 host-side 防抖测试已验证
- 真板烧录：未验证
- 真板现象：未验证

## Assumptions To Verify On Real Hardware

默认假设：

- 芯片：`STC89C52RC`
- 晶振：`11.0592 MHz`
- 烧录工具：`stcgal`
- 下载方式：STC 串口 bootloader

必须先确认：

- 学习板实际芯片型号是否就是 `STC89C52RC`
- 板上晶振是否真是 `11.0592 MHz`
- `P2.0` 是否真的接 LED
- `P3.1` 是否被用于 UART0 `TXD`
- 学习板按键是否真的接在 `P3.1`

## Critical Risk

当前 `04_key_debounce` 的板级默认映射是：

- key: `P3.1`
- uart tx: `P3.1 / TXD`

这在经典 8051 引脚定义下是冲突的。

结论：

- 在真实板子接线确认前，不能声称 `04_key_debounce` 的 LED 现象和 UART 文本输出可以同时成立
- 若板子按键确实占用 `P3.1`，应二选一：
  - 方案 A：把按键板级映射改到非 UART 引脚
  - 方案 B：保留按键在 `P3.1`，但关闭 UART 文本输出，仅验证 LED 翻转

## Example Validation Checklist

### 01_blink

通过条件：

- 烧录成功
- 上电后 LED 周期性翻转

先查：

- `P2.0` 是否真接 LED
- LED 极性是否与预期一致
- 晶振是否匹配

### 03_uart_tx

默认参数：

- `4800 8N1`
- `11.0592 MHz`
- `P3.1 / TXD`

通过条件：

- 串口工具持续看到：

```text
uart ok
```

先查：

- USB 转串口是否接到 `P3.1 / TXD`
- 是否共地
- 串口工具是否设为 `4800 8N1`
- 晶振是否真是 `11.0592 MHz`

### 04_key_debounce

当前软件预期：

- 稳定按下一次，LED 翻转一次
- 若 UART 与按键不冲突，还会输出：

```text
key press
key release
```

先查：

- 按键真实接线是不是 `P3.1`
- 若是 `P3.1`，是否已禁用 UART 文本输出或改过板级映射
- 按键是否为低电平按下
- `P2.0` 是否真接 LED

## Flash Commands

探测工具链：

```bash
mcs51/scripts/probe-toolchain.sh
```

构建全部：

```bash
mcs51/scripts/check.sh
```

烧录示例：

```bash
mcs51/scripts/flash.sh 03_uart_tx /dev/ttyUSB0
mcs51/scripts/flash.sh 04_key_debounce /dev/ttyUSB0
```

## Before Starting LCD1602

建议在进入 `02_lcd1602` 之前，先完成下面两件事：

- 确认真实板子 LED / KEY / UART 的实际占用脚位
- 将 `03_uart_tx` 与 `04_key_debounce` 至少完成一次真板现象验证
