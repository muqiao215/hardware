# Compatibility

Version: `v2.0.0-soft-validated`

## Supported MCU / Board Profile

- MCU: `STM32F103C8T6`
- Board profile: `Blue Pill` style board
- Active board layer:
  - `boards/stm32f103c8t6/`

## Supported Host Workflow

- Host type: Raspberry Pi / Linux
- Build system:
  - `cmake`
  - `ninja`
- Compiler:
  - `arm-none-eabi-gcc (15:14.2.rel1-1) 14.2.1 20241119`
- ST-Link tools:
  - `st-info v1.8.0`

## Expected Repository Structure

- `examples/` for runnable assemblies
- `boards/` for board definition and memory/startup linkage
- `common/core/` for MCU shared core layer
- `common/drivers/` for peripheral-side drivers
- `common/components/` for device-level drivers
- `common/cli/` for command-input assembly helpers
- `common/utils/` for host-testable pure logic

## Current Validation Scope

Validated in this environment:

- all 9 examples build
- all 9 examples produce firmware artifacts
- host-side tests pass for extracted pure logic modules

Not yet validated in this environment:

- ST-Link connection
- flashing
- real-board runtime observations

## Known Compatibility Limits

- Current repo copy of some legacy chapter directories contains placeholder files
- The current `7z` installation can list RAR5 archives but cannot extract them
- Real hardware behavior may vary with board clone quality, wiring, pull-up choices, OLED module variant, and serial adapter wiring
