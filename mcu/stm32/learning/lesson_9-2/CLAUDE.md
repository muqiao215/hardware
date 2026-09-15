# CLAUDE.md

## Project Overview

**Lesson 9-2**：串口发送+接收

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
- `src/Serial.c` / `include/Serial.h`（USART1 收发，PA9/PA10）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- 串口：PA9(TX) -> USB-TTL RX，PA10(RX) <- USB-TTL TX，GND 共地（默认 9600）
- OLED：PB8(SCL)，PB9(SDA)
