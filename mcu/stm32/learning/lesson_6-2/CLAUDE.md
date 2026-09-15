# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

---

## Project Overview

**Lesson 6-1**: TIM2定时器定时中断 (Timer Interrupt)

## Build Commands

```bash
# Configure
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -G Ninja

# Build
cmake --build build

# Flash
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

## Key Files

| File | Description |
|------|-------------|
| `src/Timer.c` | Timer initialization and interrupt handler |
| `src/main_timer.c` | Main application |
| `include/Timer.h` | Timer function declarations |

## Timer Configuration

| Parameter | Value | Description |
|-----------|-------|-------------|
| APB Clock | 72MHz | System clock |
| PSC | 7200-1 | Prescaler |
| ARR | 10000-1 | Auto-reload |
| Interrupt | 10ms | Update event interrupt |

## Hardware

- OLED: PB8 (SCL), PB9 (SDA)
