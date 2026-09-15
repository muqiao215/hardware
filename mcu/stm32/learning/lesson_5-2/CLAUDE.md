# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

---

## Project Overview

**Lesson 5-2: Rotary Encoder Counter** for STM32F103C8T6 (Blue Pill)

This project demonstrates **quadrature encoder decoding** using EXTI interrupts. The encoder's two-phase output signals (A and B) are used to determine rotation direction and count.

**Hardware Wiring:**
| Component | Pin | Description |
|-----------|-----|-------------|
| Encoder A (CLK) | PB0 | Phase A signal (EXTI0) |
| Encoder B (DT) | PB1 | Phase B signal (EXTI1) |
| Encoder SW | Not used | Push button |
| OLED SCL | PB8 | I2C clock (4-pin version) |
| OLED SDA | PB9 | I2C data (4-pin version) |

---

## Quadrature Encoding Theory

Two signals with 90-degree phase difference enable direction detection:

```
Clockwise rotation (A leads B):
A: ──┐  ┌──┐  ┌──
    └──┘  └──┘
B: ───┐  ┌──┐  ┌─
     └──┘  └──┘

Counter-clockwise rotation (B leads A):
A: ───┐  ┌──┐  ┌─
     └──┘  └──┘
B: ──┐  ┌──┐  ┌──
    └──┘  └──┘
```

**Direction Detection Logic:**
| Event | Other Phase | Direction |
|-------|-------------|-----------|
| A falling edge | B = LOW | Counter-CW (-) |
| A falling edge | B = HIGH | Clockwise (+) |
| B falling edge | A = LOW | Clockwise (+) |
| B falling edge | A = HIGH | Counter-CW (-) |

---

## Learning Exercise

The `src/Encoder.c` file contains **TODO sections** for you to implement the direction detection logic in the ISRs.

Open the file and find:
```c
// ============ YOUR CODE HERE ============

// ========================================
```

Implement the direction detection based on the theory above.

---

## Commands

### Configure & Build

```bash
# Configure
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
    -DBOARD=stm32f103c8t6 \
    -DAPP=encoder \
    -G Ninja

# Build
cmake --build build

# Build + Flash combined
cmake --build build && openocd -f boards/stm32f103c8t6/openocd.cfg \
    -c "program build/firmware.elf verify reset exit"
```

---

## Architecture

```
lesson_5-2/
├── src/
│   ├── main_encoder.c   # Main application
│   ├── Encoder.c        # Rotary encoder driver (TODO: implement ISRs)
│   ├── Delay.c          # Software delay
│   └── OLED.c           # I2C OLED driver
├── include/
│   ├── Encoder.h
│   ├── Delay.h
│   └── OLED.h
└── ...
```

---

## Key Code Pattern

### EXTI Interrupt for Encoder (Direction Detection)

```c
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        // Noise filter
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
        {
            // A falling edge: check B phase
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
                Encoder_Count--;  // Counter-clockwise
            else
                Encoder_Count++;  // Clockwise (NOT IMPLEMENTED - YOUR TASK!)
        }
        EXTI_ClearITPendingBit(EXTI_Line0);  // CRITICAL: must clear!
    }
}
```

---

## Expected Behavior

After implementing the ISR logic:
- OLED displays "Num: xxxxx"
- Rotate encoder **clockwise**: number **increases**
- Rotate encoder **counter-clockwise**: number **decreases**

---

## Comparison with Lesson 5-1

| Feature | 5-1 IR Sensor | 5-2 Encoder |
|---------|---------------|-------------|
| Signals | 1 | 2 (A+B) |
| EXTI lines | 1 | 2 |
| Direction | N/A | Yes |
| Count type | Unidirectional | Bidirectional |

---

## Next Steps

After completing 5-2, you'll learn:
- **Chapter 6**: TIM Timer (hardware-based timing)
- **6-8**: Hardware Encoder Interface (TIM's encoder mode for high-speed counting)
