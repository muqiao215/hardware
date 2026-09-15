# 02_oled_i2c

目标：把原 `pi_gcc_f103_blink` OLED 动画整理成规范示例，并沉淀成 `framebuffer + ssd1306 + soft_i2c_bus` 复用链路。

接线：

- `PB8`：软件 I2C `SCL`
- `PB9`：软件 I2C `SDA`
- 设备：SSD1306 128x64 OLED，7-bit 地址通常为 `0x3C`

构建：

```bash
cmake -S examples/02_oled_i2c -B build/02_oled_i2c -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchains/arm-gcc.cmake
cmake --build build/02_oled_i2c
```

预期现象：

- OLED 初始化后循环显示笑脸/哭脸动画

产物：

- `build/02_oled_i2c/firmware.elf`
- `build/02_oled_i2c/firmware.hex`
- `build/02_oled_i2c/firmware.bin`
- `build/02_oled_i2c/firmware.map`

常见失败原因：

- OLED 模块实际地址不是 `0x3C`
- `PB8/PB9` 接反
- 屏幕供电或地线未接稳

说明：

- 这是对现有 `pi_gcc_f103_blink/` 的规范化整合，legacy 种子目录保留不动
