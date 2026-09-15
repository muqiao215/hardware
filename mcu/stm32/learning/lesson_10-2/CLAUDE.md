# CLAUDE.md

## Project Overview

**Lesson 10-2**：硬件I2C读写MPU6050

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
- `src/MyI2C.c` / `include/MyI2C.h`（硬件 I2C：I2C2 @ PB10/PB11）
- `src/MPU6050.c` / `include/MPU6050.h`
- `src/OLED.c` / `include/OLED.h`（PB8/PB9 软件 I2C）

## Wiring

- MPU6050（硬件 I2C2）：PB10(SCL)，PB11(SDA)
- OLED：PB8(SCL)，PB9(SDA)
