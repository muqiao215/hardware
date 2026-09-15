# STC89C52 Migration Plan

## Goal

Build a separate Raspberry Pi friendly migration track for the imported `MCS-51 / STC89C52` course material without disturbing the frozen STM32F103 v2 baseline.

End state for the first 51 migration version:

- Keep upstream `mcs51/Learn_STC89C52/` intact as source material.
- Add a separate 8051/STC build layout under `mcs51/`.
- Build migrated 51 examples from the Raspberry Pi command line.
- Generate Intel HEX artifacts suitable for STC flashing.
- Add no-hardware checks for build and host-testable logic.
- Add a hardware-pending checklist for serial/STC flashing and board behavior.

Non-goal:

- Do not merge 8051 assumptions into the STM32 `boards/`, `common/`, `examples/`, or `cmake/stm32f103_example.cmake` layers.
- Do not claim STC89C52 hardware flashing success without a connected board and serial bootloader path.

## Current Source Material

Imported upstream:

- `https://github.com/jjejdhhd/Learn_STC89C52`
- Upstream HEAD: `cf3a6b0fbcaad2bc8ceb8c3eb1386b794ac86485`
- Local path: `mcs51/Learn_STC89C52/`

Observed source structure:

- chapter readmes from `01_简介及点灯` through `15_红外遥控与外部中断`
- raw Keil/C51-style `.c/.h` source files in many lesson directories
- `.zip` archives for several complete lesson projects
- source code includes Keil-style `#include <REGX52.H>` and likely `sbit` / SFR-specific syntax

## Toolchain Assumption

Preferred Raspberry Pi side toolchain:

- `sdcc` for 8051 C compilation
- `packihx` for Intel HEX generation
- `stcgal` for STC serial bootloader flashing
- `python3` for scripts

Current host observation:

- `python3` exists
- `sdcc`, `packihx`, and `stcgal` were not found on `PATH`

## Target Layout

Active 51 layout should stay narrow until the build path is proven:

```text
mcs51/
  Learn_STC89C52/          # imported upstream source material, keep intact
  README.md
  UPSTREAM.md
  plans/                  # optional per-track notes, if needed
  boards/
    stc89c52rc/
      board.h
      pinmap.h
      README.md
  common/
    core/
      reg52_compat.h      # SDCC/Keil compatibility shim
    drivers/
      uart1.*
      timer0.*
      gpio.*
    components/
      lcd1602.*
      nixie.*
    utils/
      delay.*
      debounce.*
      ringbuf.*           # reuse/copy only if target-compatible and useful
  examples/
    01_blink/
    02_lcd1602/
    03_uart_tx/
    04_key_debounce/
  scripts/
    build.sh
    build-all.sh
    check.sh
    flash.sh
    probe-toolchain.sh
    package-validation-bundle.sh
```

## Migration Phases

### Phase 1: Toolchain And Build Spike

Goal:

- Establish the 8051 command-line compiler path on Raspberry Pi.
- Do not add CMake yet.
- Use plain shell scripts until SDCC, packihx, and the first `.hex` artifact are proven.

Tasks:

- Add `mcs51/scripts/probe-toolchain.sh`
- Add `mcs51/scripts/build.sh`
- Document install commands for `sdcc` and `stcgal`
- Create the smallest `mcs51/examples/01_blink` using SDCC-compatible headers
- Generate `.ihx` and `.hex`
- Keep STM32 `scripts/check.sh` unchanged except for optional high-level source-material checks

Exit gate:

```bash
mcs51/scripts/probe-toolchain.sh
mcs51/scripts/build-all.sh
```

Expected status if tools are missing:

- `toolchain_pending`

Implementation constraint:

- Use a thin SDCC-native header for `01_blink`.
- Do not attempt a full `REGX52.H` compatibility layer in Phase 1.

### Phase 2: Keil-To-SDCC Compatibility Layer

Goal:

- Avoid editing copied lesson code repeatedly just to replace compiler syntax.

Tasks:

- Add a small `reg52_compat.h` strategy:
  - map `REGX52.H` assumptions to SDCC-compatible registers where possible
  - document unsupported Keil-specific features
- Decide whether each migrated example:
  - includes a compatibility wrapper
  - or is rewritten as clean SDCC code using the original as reference

Exit gate:

- `01_blink` builds without Keil
- compatibility decisions are documented in `mcs51/README.md`

### Phase 3: First Standard Examples

Goal:

- Create a small, maintainable 51 example set instead of bulk-copying all lessons.

Initial target examples:

- `01_blink`
- `03_uart_tx`
- `04_key_debounce`
- `02_lcd1602`

Why these first:

- LED proves SFR and pin control.
- UART proves serial debug path and future CLI/logging.
- Key debounce lets us reuse host-testable logic discipline from STM32 v2.
- LCD1602 tests component extraction and course continuity, but should wait until blink and UART are stable.

Exit gate:

```bash
mcs51/scripts/build-all.sh
```

Each example should produce at least:

- `firmware.ihx`
- `firmware.hex`
- size/map-like output if available from SDCC tooling

### Phase 4: 51 Check Gate

Goal:

- Give the 51 track its own no-hardware gate.

Tasks:

- Add `mcs51/scripts/check.sh`
- Check:
  - toolchain availability
  - all active examples build
  - README exists per example
  - expected `.ihx/.hex` artifacts exist
  - no generated artifacts are committed
  - source-material directory remains intact

Exit gate:

```bash
mcs51/scripts/check.sh
```

### Phase 5: Flash And Hardware Pending Handoff

Goal:

- Make STC89C52 board validation transferable.

Tasks:

- Add `mcs51/HARDWARE_PENDING.md`
- Add `mcs51/REALBOARD_VALIDATION_CHECKLIST.md`
- Add `mcs51/scripts/flash.sh`
- Add `mcs51/scripts/package-validation-bundle.sh`

Expected flash path:

```bash
stcgal -P stc89 -p <serial-port> build/<example>/firmware.hex
```

This must remain `hardware_pending` until a real STC board and serial bootloader wiring are verified.

### Phase 6: Expand Course Coverage

Only after the first 51 build/check path is stable, migrate additional lessons:

- matrix keyboard
- timer interrupt
- LED matrix
- DS1302
- buzzer
- AT24C02 I2C
- DS18B20
- DC motor
- AD/DA
- IR remote / external interrupt

Policy:

- Prefer reusable modules over per-example copy-paste.
- Keep examples thin.
- Add host-side tests for pure logic where practical.

## Acceptance Criteria For 51 v1

- `mcs51/` has a documented independent build workflow.
- At least `01_blink`, `02_lcd1602`, and `03_uart_tx` build under SDCC.
- No changes are required to the frozen STM32 v2 build system.
- `mcs51/scripts/check.sh` is the primary no-hardware gate for 51.
- Hardware flashing is clearly marked `hardware_pending` unless actually tested.

## Immediate Next Action

Install or confirm SDCC/STC tooling, then drive Phase 1 to a real `.hex` artifact:

```bash
sudo apt install sdcc
python3 -m pip install --user stcgal
```

If install is not allowed in the environment, record `toolchain_pending` and still scaffold the build layout.
