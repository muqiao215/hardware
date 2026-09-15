# 03_uart_tx

最小串口发送示例。

目标：

- 在 `STC89C52RC` 上使用 UART0 周期性发送固定字符串
- 只做发送，不做接收，不做中断，不做命令行

固定参数：

- 芯片：`STC89C52RC`
- 系统时钟：`11.0592 MHz`
- 串口：`UART0`
- 波特率：`4800 8N1`
- 波特率发生方式：`Timer1 mode 2`
- 发送方式：轮询 `TI`

接线：

- `P3.1 / TXD` -> USB 转串口模块 `RXD`
- `GND` -> USB 转串口模块 `GND`
- 若后续要做接收，再接 `P3.0 / RXD`

预期现象：

- 烧录并上电后，串口工具应周期性看到：

```text
uart ok
```

串口工具建议：

- 波特率：`4800`
- 数据位：`8`
- 停止位：`1`
- 校验位：`None`

构建：

```bash
mcs51/scripts/build.sh 03_uart_tx
```

烧录：

```bash
mcs51/scripts/flash.sh 03_uart_tx /dev/ttyUSB0
```

看不到输出时先查：

- 晶振是否真是 `11.0592MHz`
- USB 转串口是否接到了 `P3.1/TXD`
- 串口工具是否设为 `4800 8N1`
- 共地是否接好
- 板子是否按 STC 下载流程重新上电进入 bootloader
