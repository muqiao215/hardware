# 05_adc_polling

目标：迁移 ADC 单通道轮询采样。

接线：

- `PA0 / ADC1_IN0`：模拟输入
- `PC13`：根据阈值亮灭

预期现象：

- 模拟量大于中值时 LED 点亮

产物：

- `build/05_adc_polling/firmware.elf`
- `build/05_adc_polling/firmware.hex`
- `build/05_adc_polling/firmware.bin`
- `build/05_adc_polling/firmware.map`

常见失败原因：

- `PA0` 输入电压超出 `0V~3.3V`
- 模拟源未共地
