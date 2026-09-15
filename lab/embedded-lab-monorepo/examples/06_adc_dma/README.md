# 06_adc_dma

目标：迁移 ADC + DMA 多通道采样。

接线：

- `PA0 / ADC1_IN0`
- `PA1 / ADC1_IN1`

预期现象：

- DMA 循环搬运两路采样数据，示例用比较结果控制 LED

产物：

- `build/06_adc_dma/firmware.elf`
- `build/06_adc_dma/firmware.hex`
- `build/06_adc_dma/firmware.bin`
- `build/06_adc_dma/firmware.map`

常见失败原因：

- 两路输入悬空，结果不稳定
- 采样源阻抗过高导致结果漂移
