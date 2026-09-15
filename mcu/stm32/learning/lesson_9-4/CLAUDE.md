# CLAUDE.md

## Project Overview

**Lesson 9-4**：串口收发文本数据包

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main.c`
- `src/Serial.c` / `include/Serial.h`
- `src/SerialPacket.c` / `include/SerialPacket.h`
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- 串口：PA9(TX) / PA10(RX)（默认 9600）
- OLED：PB8(SCL)，PB9(SDA)

## 串口助手发送格式

- 兼容两种输入：
  - 推荐：直接发送 `LED_ON` 或 `LED_OFF`（可不加换行；加 `\n`/`\r\n` 也可以）
  - 兼容课件包格式：`@LED_ON\r\n` / `@LED_OFF\r\n`
- 强力自愈：如果之前发错导致错位，重新发送一次带 `@` 的标准包头（如 `@LED_ON\r\n`）会强制重同步
- 回传：
  - `LED_ON_OK` / `LED_OFF_OK` / `ERROR_COMMAND`
