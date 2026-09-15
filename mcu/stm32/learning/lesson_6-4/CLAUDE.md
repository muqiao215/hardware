# CLAUDE.md

## Project Overview

**Lesson 6-4**：PWM驱动舵机（连续扫动 + 挡位调速 + LED 同步）

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=servo \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_servo.c`（持续扫动 + Key1 挡位）
- `src/Servo.c` / `include/Servo.h`（舵机角度）
- `src/PWM.c` / `src/PWM.h`（TIM2_CH2@PA1 舵机 + TIM2_CH1@PA0 LED）
- `src/Key.c` / `include/Key.h`（PB1）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- 舵机（SG90）：PA1（信号），5V（供电），GND（共地）
- LED（串电阻）：正极 -> PA0，负极 -> GND
- Key1：PB1
- OLED：PB8(SCL)，PB9(SDA)
