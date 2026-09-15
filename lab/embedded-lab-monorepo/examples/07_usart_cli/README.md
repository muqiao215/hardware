# 07_usart_cli

目标：把 USART 课程示例从简单回显收成可复用的 CLI 骨架。

接线：

- `PA9`：USART1_TX
- `PA10`：USART1_RX

串口参数：

- `115200 8N1`

预期现象：

- 上电打印 `STM32F103 USART CLI ready`
- 输入 `help` 可列出命令
- 输入 `led on` / `led off` / `led toggle` 可控制板载 LED
- 输入 `echo hello stm32` 可回显参数

产物：

- `build/07_usart_cli/firmware.elf`
- `build/07_usart_cli/firmware.hex`
- `build/07_usart_cli/firmware.bin`
- `build/07_usart_cli/firmware.map`

常见失败原因：

- USB 转串口与板子未共地
- 串口参数不是 `115200 8N1`
- 实际接反了 `TX/RX`

说明：

- 当前已拆成 `cli_input_pump + ringbuf + cli_parser + cli_dispatch + usart_cli_port`
