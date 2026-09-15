# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

---

## Project Overview

**Lesson 3-3: Multi-App Demo Project** for STM32F103C8T6 (Blue Pill)

This project demonstrates GPIO output control with buzzer, I2C OLED display, and LED running lights. Supports **multiple applications** that can be selected at build time.

**Hardware Wiring:**
| Component | Pin | Description |
|-----------|-----|-------------|
| Buzzer | PB12 | Active-low (LOW = ON) |
| OLED SCL | PB8 | I2C clock (4-pin version) |
| OLED SDA | PB9 | I2C data (4-pin version) |
| LED0-LED7 | PA0-PA7 | Active-low running lights |

---

## Multi-Application Support

This project uses CMake's `-DAPP=` variable to select which program to build.

### Available Applications

| APP Name | Main File | Description |
|----------|-----------|-------------|
| `grenade` | `main_grenade.c` | Grenade explosion simulator (countdown + LED chase) |
| `oled_demo` | `main_oled_demo.c` | OLED boot animation + status display |

### Adding New Applications

1. Create `src/main_<name>.c` (e.g., `main_led_test.c`)
2. CMake will auto-detect it - no need to modify CMakeLists.txt
3. Build with `-DAPP=<name>` (e.g., `-DAPP=led_test`)

### File Naming Convention

```
src/
├── main_grenade.c     # APP=grenade
├── main_oled_demo.c   # APP=oled_demo
├── main_led_test.c    # APP=led_test (example)
├── Delay.c            # Shared module
└── OLED.c             # Shared module
```

---

## Commands

### Configure & Build

```bash
# List available applications (omit -DAPP)
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 -G Ninja

# Configure with specific application
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=grenade \
    -G Ninja

# Build
cmake --build build

# Switch application (must reconfigure)
cmake -S . -B build -DAPP=oled_demo
cmake --build build
```

### Flash

```bash
# Flash via ST-Link
openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"

# Build + Flash combined
cmake --build build && openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

### Quick Switch Script

```bash
# One-liner to switch app, build, and flash
APP=grenade && cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 -DAPP=$APP -G Ninja && \
    cmake --build build && \
    openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

---

## Architecture

```
lesson_3-3/
├── src/
│   ├── main_grenade.c   # Grenade explosion app
│   ├── main_oled_demo.c # OLED demo app
│   ├── Delay.c          # Shared: Software delay (SysTick-based)
│   └── OLED.c           # Shared: 4-pin I2C OLED driver
├── include/
│   ├── Delay.h
│   └── OLED.h
├── drivers/             # STM32F10x StdPeriph Library V3.5.0
├── boards/stm32f103c8t6/
│   ├── openocd.cfg
│   └── STM32F103C8Tx_FLASH.ld
├── cmake/
│   ├── toolchains/arm-gcc.cmake
│   └── boards/stm32f103c8t6.cmake
└── .vscode/
    ├── launch.json
    └── tasks.json
```

---

## Key Code Patterns

### GPIO Output Control (Buzzer)
```c
// 1. Enable clock for the GPIO port
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

// 2. Configure pin as push-pull output
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);

// 3. Control output (active-low buzzer)
GPIO_ResetBits(GPIOB, GPIO_Pin_12);  // ON
GPIO_SetBits(GPIOB, GPIO_Pin_12);    // OFF
```

### LED Pattern Control (PA0-PA7)
```c
// Initialize all 8 LEDs
GPIO_InitStructure.GPIO_Pin = 0x00FF;  // PA0-PA7
GPIO_Init(GPIOA, &GPIO_InitStructure);

// Pattern control (active-low: 0=ON, 1=OFF)
GPIO_Write(GPIOA, ~pattern);           // Set pattern
GPIO_Write(GPIOA, ~(1 << i));          // Single LED on
GPIO_Write(GPIOA, 0x00FF);             // All off
GPIO_Write(GPIOA, 0x0000);             // All on
```

### OLED Display API
```c
OLED_Init();                          // Initialize display
OLED_Clear();                         // Clear screen
OLED_ShowString(row, col, "text");    // Show text (row 1-4, col 1-16)
OLED_ShowNum(row, col, num, len);     // Show decimal number
OLED_ShowHexNum(row, col, num, len);  // Show hex number
OLED_ShowChar(row, col, char);        // Show single character
```

---

## Debugging

Debug configuration uses Cortex-Debug + OpenOCD. Press F5 to start debugging.

| Key | Action |
|-----|--------|
| F5 | Continue/Start debug |
| F10 | Step over |
| F11 | Step into |
| Shift+F11 | Step out |
| F9 | Toggle breakpoint |

**SVD File:** `svd/STM32F103.svd` enables peripheral register viewing.

---

## WSL2 USB Forwarding

ST-Link requires USB forwarding from Windows:
```powershell
# Windows PowerShell (Admin)
usbipd list                           # Find ST-Link BUSID
usbipd bind --busid <BUSID>           # First time only
usbipd attach --wsl --busid <BUSID>   # Forward to WSL
```

Verify in WSL: `lsusb | grep -i st-link`

---

## Adding Shared Modules

To add a new shared module (used by all apps):

1. Add `src/NewModule.c` and `include/NewModule.h`
2. Update CMakeLists.txt `COMMON_SOURCES`:
```cmake
set(COMMON_SOURCES
    src/Delay.c
    src/OLED.c
    src/NewModule.c  # Add here
)
```
3. Rebuild: `cmake --build build`
