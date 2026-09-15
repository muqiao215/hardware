# 04_tim_pwm

目标：迁移定时器 PWM 基础示例。

接线：

- `PA0 / TIM2_CH1`：PWM 输出

预期现象：

- 占空比在 0% 到 100% 循环变化，可接 LED 观察呼吸效果

产物：

- `build/04_tim_pwm/firmware.elf`
- `build/04_tim_pwm/firmware.hex`
- `build/04_tim_pwm/firmware.bin`
- `build/04_tim_pwm/firmware.map`

常见失败原因：

- 负载没有限流
- `PA0` 与其它实验复用，接线冲突
