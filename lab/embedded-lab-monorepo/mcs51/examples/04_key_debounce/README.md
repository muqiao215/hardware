# 04_key_debounce

单按键防抖示例。

目标：

- 把原始抖动输入转换成稳定的 `press` / `release` 事件
- 复用 `common/utils/debounce.*` 纯逻辑模块
- 示例层只负责读取引脚、调用状态机、输出现象

固定参数：

- 芯片：`STC89C52RC`
- 按键：当前默认由 `boards/stc89c52rc/board.h` 映射
- 按下电平：低电平
- LED：当前默认由 `boards/stc89c52rc/board.h` 映射到 `P2.0`
- 串口：`4800 8N1`
- 防抖阈值：连续 3 次采样一致后确认状态变化

接线：

- 按键一端接 `P3.1`
- 按键另一端接 `GND`
- `P3.1` 需要上拉；8051 P3 口通常有准双向上拉，但真实板子仍以原理图为准
- `P2.0` 接 LED，具体亮灭极性以板子为准
- `P3.1 / TXD` 与 UART0 TX 冲突：如果真实板子使用 `P3.1` 做按键，就不能同时用 UART0 TX 验证串口输出

注意：

- 当前示例保留串口输出用于软件观察
- 默认板级映射把按键别名也指向了 `P3.1`
- 由于 `P3.1` 同时是经典 8051 `TXD`，这在真实硬件上很可能冲突
- 真板验证前应确认学习板按键实际接线；如果按键占用 `P3.1`，下一步应把按键迁到不冲突的引脚，或关闭串口输出只看 LED

预期现象：

- 稳定按下一次按键时，`P2.0` 翻转一次
- 若串口 TX 未被按键占用，串口工具应看到：

```text
key press
key release
```

构建：

```bash
mcs51/scripts/build.sh 04_key_debounce
```

主机侧逻辑测试：

```bash
mcs51/scripts/test-host.sh
```

烧录：

```bash
mcs51/scripts/flash.sh 04_key_debounce /dev/ttyUSB0
```
