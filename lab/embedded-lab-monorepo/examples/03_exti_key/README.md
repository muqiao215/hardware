# 03_exti_key

目标：迁移 `EXTI` 外部中断课程为最小按键中断示例。

接线：

- `PA0`：按键输入，上拉，下降沿触发
- `PC13`：LED，按一次翻转一次

预期现象：

- 每按一次按键，LED 翻转一次

产物：

- `build/03_exti_key/firmware.elf`
- `build/03_exti_key/firmware.hex`
- `build/03_exti_key/firmware.bin`
- `build/03_exti_key/firmware.map`

常见失败原因：

- `PA0` 没有正确上拉
- 按键抖动过大时，当前 `debounce` 阈值可能需要按真板特性微调

说明：

- `EXTI0_IRQHandler` 只记录 `g_exti_count` 并清除中断标志
- LED 翻转由主循环里的 `debounce` 稳定按下事件驱动
- `g_exti_count` 只记录 EXTI 触发次数，便于调试器观察
