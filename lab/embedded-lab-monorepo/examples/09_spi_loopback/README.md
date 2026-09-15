# 09_spi_loopback

目标：迁移 SPI 主机基础读写。

接线：

- `PA4`：软件控制 `NSS`
- `PA5`：`SPI1_SCK`
- `PA6`：`SPI1_MISO`
- `PA7`：`SPI1_MOSI`

预期现象：

- 若 `MOSI` 与 `MISO` 做回环，收到 `0xA5` 时 LED 常亮

产物：

- `build/09_spi_loopback/firmware.elf`
- `build/09_spi_loopback/firmware.hex`
- `build/09_spi_loopback/firmware.bin`
- `build/09_spi_loopback/firmware.map`

常见失败原因：

- `MOSI/MISO` 没有真正回环
- 从设备模式或时钟极性设置不匹配
