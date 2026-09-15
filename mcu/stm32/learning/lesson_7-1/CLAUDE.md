# CLAUDE.md

## Project Overview

**Lesson 7-1**：AD单通道

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
- `src/AD.c` / `include/AD.h`（ADC1 单通道：PA0 / ADC_Channel_0）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- AD 输入：PA0
- OLED：PB8(SCL)，PB9(SDA)
