# CLAUDE.md

## Project Overview

**Lesson 8-1**：DMA数据转运

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=dma \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_dma.c`
- `src/MyDMA.c` / `include/MyDMA.h`（DMA1_Channel1：数组 DataA -> DataB）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- OLED：PB8(SCL)，PB9(SDA)
