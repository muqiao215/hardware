# CLAUDE.md

## Project Overview

**Lesson 11-1**：软件SPI读写W25Q64

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
- `src/MySPI.c` / `include/MySPI.h`（软件 SPI：PA4/PA5/PA6/PA7）
- `src/W25Q64.c` / `include/W25Q64.h`
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- W25Q64（软件 SPI）：PA4(CS)，PA5(SCK)，PA6(MISO)，PA7(MOSI)
- OLED：PB8(SCL)，PB9(SDA)
