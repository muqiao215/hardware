# CLAUDE.md

## Project Overview

**Lesson 6-6**：输入捕获模式测频率

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=ic \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_ic.c`
- `src/PWM.c` / `include/PWM.h`（TIM2_CH1 @ PA0，提供测试 PWM 信号）
- `src/IC.c` / `include/IC.h`（TIM3_CH1 @ PA6，输入捕获测频）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- PWM 输出：PA0（TIM2_CH1）
- 输入捕获：PA6（TIM3_CH1），把 PA0 外接到 PA6
- OLED：PB8(SCL)，PB9(SDA)
