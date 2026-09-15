# 08_i2c_hw

目标：保留独立的硬件 I2C 总线示例，和 OLED 的软件 I2C 示例分开。

接线：

- `PB6`：I2C1_SCL
- `PB7`：I2C1_SDA

预期现象：

- 当前逻辑会对 `0x3C` 做 probe
- 若设备应答，LED 常亮；未应答则 LED 持续翻转

产物：

- `build/08_i2c_hw/firmware.elf`
- `build/08_i2c_hw/firmware.hex`
- `build/08_i2c_hw/firmware.bin`
- `build/08_i2c_hw/firmware.map`

常见失败原因：

- 总线上没有 `0x3C` 设备
- 缺少上拉电阻
- `PB6/PB7` 接线错误

说明：

- 当前示例先验证硬件 I2C 初始化和最小 probe 接口
