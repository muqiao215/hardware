# Changelog

## Monorepo Consolidation - 2026-04-13

### Added

- Imported `rpi_hpad/` into the tracked repository as a sibling project
- Added root aggregate gate:
  - `scripts/check-all.sh`
- Added monorepo root documentation for:
  - STM32F103
  - MCS-51 / STC89C52
  - Raspberry Pi HPAD

### Changed

- The repository is now a monorepo rather than a STM32-only mainline with side notes
- Root `README.md` now acts as the entrypoint for all three subprojects
- Root `.gitignore` now covers Python package and virtualenv artifacts used by `rpi_hpad/`

### Verified

- `mcs51/scripts/check.sh`
- `rpi_hpad/scripts/check.sh`
- `scripts/check-all.sh`

### Pending

- Hardware validation remains pending in each subproject until the corresponding real board is connected
## v2.0.0-soft-validated - 2026-04-11

### Added

- Unified GNU Arm Embedded + CMake + Ninja bare-metal workflow for Raspberry Pi development
- Imported `MCS-51 / STC89C52` source-material track under `mcs51/`
- Nine independently buildable examples under `examples/`
- Shared layered runtime layout:
  - `boards/`
  - `common/core/`
  - `common/drivers/`
  - `common/components/`
  - `common/cli/`
  - `common/utils/`
- Host-side logic tests for:
  - CLI parser and dispatch
  - CLI input pump
  - ring buffer
  - debounce state machine
  - framebuffer drawing
- Release-facing docs:
  - `RELEASE_NOTES.md`
  - `HARDWARE_PENDING.md`
  - `COMPATIBILITY.md`
  - `REALBOARD_VALIDATION_CHECKLIST.md`
- Validation bundle packaging script:
  - `scripts/package-validation-bundle.sh`
- Version marker:
  - `VERSION`

### Changed

- `07_usart_cli` was slimmed into:
  - `cli_input_pump`
  - `ringbuf`
  - `cli_parser`
  - `cli_dispatch`
  - `usart_cli_port`
- `03_exti_key` now uses `EXTI + debounce` layering instead of direct ISR-side LED control
- Example CMake integration now follows a unified `BOARD + MODULES` contract
- `scripts/check.sh` is now the primary no-hardware release gate

### Verified

- All 9 examples build successfully with `-Werror`
- Each example produces:
  - `firmware.elf`
  - `firmware.hex`
  - `firmware.bin`
  - `firmware.map`
- Host-side tests pass on the Raspberry Pi build node
- Validation bundles can be generated from local build artifacts
- A real-board validation execution checklist is now present for handoff

### Pending

- ST-Link probe still reports `Found 0 stlink programmers`
- Flashing and real-board runtime observation remain `hardware_pending`
- Debounce threshold and board-level pin behavior still require real hardware validation
