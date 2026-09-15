# CLAUDE.md

## Project Overview

**Lesson 6-5**：PWM驱动直流电机

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=motor \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_motor.c`
- `src/Motor.c` / `include/Motor.h`（PA4/PA5 方向控制，PA2 PWM）
- `src/PWM.c` / `include/PWM.h`（TIM2_CH3 @ PA2）
- `src/Key.c` / `include/Key.h`（PB1）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- 电机驱动（L298N 等）：ENA(PWM)=PA2，IN1=PA4，IN2=PA5
- OLED：PB8(SCL)，PB9(SDA)
- KEY1：PB1
