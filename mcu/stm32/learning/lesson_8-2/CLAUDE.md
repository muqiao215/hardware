# CLAUDE.md

## Project Overview

**Lesson 8-2**：DMA+AD多通道

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=ad \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_ad.c`
- `src/AD.c` / `include/AD.h`（ADC1 扫描 + DMA1_Channel1 循环转运到 `AD_Value[4]`）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- AD 输入：PA0/PA1/PA2/PA3
- OLED：PB8(SCL)，PB9(SDA)
