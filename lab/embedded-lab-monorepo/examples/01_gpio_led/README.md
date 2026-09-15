# 01_gpio_led

目标：把原 `GPIO` 课程里的最小 LED 输出迁移成树莓派可构建的裸机示例。

接线：

- `PC13`：板载 LED，低电平亮

构建：

```bash
cmake -S examples/01_gpio_led -B build/01_gpio_led -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchains/arm-gcc.cmake
cmake --build build/01_gpio_led
```

烧录：

```bash
scripts/flash.sh 01_gpio_led
```

预期现象：

- LED 约 250ms 翻转一次

产物：

- `build/01_gpio_led/firmware.elf`
- `build/01_gpio_led/firmware.hex`
- `build/01_gpio_led/firmware.bin`
- `build/01_gpio_led/firmware.map`

常见失败原因：

- 板子并非 `PC13` 板载 LED 设计
- ST-Link 未连接，`scripts/probe-stlink.sh` 返回 `Found 0 stlink programmers`
