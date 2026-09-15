# STM32 Timer Interrupt Project

## Hardware Connections
- OLED Display (4-pin I2C):
  - SCL -> PB8
  - SDA -> PB9
  - VCC -> 3.3V
  - GND -> GND

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

## Expected Behavior
- OLED displays "Lesson 6-1" and "Timer Interrupt"
- Num variable increments every 1 second
- Timer interrupt triggers every 10ms (PSC=7200, ARR=10000)
