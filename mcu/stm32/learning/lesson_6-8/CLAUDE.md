# CLAUDE.md

## Project Overview

**Lesson 6-8**：编码器接口测速

## Build

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=encoder_speed \
    -G Ninja
cmake --build build
```

## Flash

```bash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

- `src/main_encoder_speed.c`
- `src/Encoder.c` / `include/Encoder.h`（TIM3 编码器接口，PA6/PA7）
- `src/Timer.c` / `include/Timer.h`（定时中断周期读取编码器增量）
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- 编码器 A/B 相：PA6（TIM3_CH1），PA7（TIM3_CH2）
- OLED：PB8(SCL)，PB9(SDA)
