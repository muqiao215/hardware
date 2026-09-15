# STC89C52 Migration Progress

## 2026-04-11

### Planning Session

Status: planned / toolchain_pending / hardware_pending.

Completed:

- Imported upstream `Learn_STC89C52` source material under `mcs51/Learn_STC89C52/`.
- Confirmed this should be a separate migration line from the frozen STM32F103 v2 baseline.
- Read representative source and confirmed Keil/C51 assumptions such as `REGX52.H`.
- Checked local tool availability:
  - `python3` exists
  - `sdcc` not found
  - `packihx` not found
  - `stcgal` not found
- Created this separate plan set:
  - `plans/mcs51_migration/task_plan.md`
  - `plans/mcs51_migration/findings.md`
  - `plans/mcs51_migration/progress.md`
  - `plans/mcs51_migration/files.md`

Decision:

- Do not mix STC89C52 into the existing STM32 `cmake/`, `boards/`, or `scripts/check.sh` implementation.
- Create a dedicated `mcs51/` build/check/flash track.

Next action:

- Implement Phase 1:
  - `mcs51/scripts/probe-toolchain.sh`
  - `mcs51/scripts/build.sh`
  - minimal `mcs51/examples/01_blink`
  - `mcs51/scripts/build-all.sh`
  - clear `toolchain_pending` behavior if SDCC is still unavailable

---

### Phase 1 Scaffold

Status: phase1_scaffolded / toolchain_pending / hardware_pending.

Completed:

- Added minimal MCS-51 shell build path:
  - `mcs51/scripts/probe-toolchain.sh`
  - `mcs51/scripts/build.sh`
  - `mcs51/scripts/build-all.sh`
  - `mcs51/scripts/check.sh`
  - `mcs51/scripts/flash.sh`
- Added first board profile:
  - `mcs51/boards/stc89c52rc/README.md`
  - `mcs51/boards/stc89c52rc/board.h`
- Added thin SDCC-native header:
  - `mcs51/common/core/stc89c52_min.h`
- Added first minimal example:
  - `mcs51/examples/01_blink/README.md`
  - `mcs51/examples/01_blink/main.c`

Boundary decisions:

- No CMake for 51 Phase 1.
- No thick `REGX52.H` compatibility layer.
- `01_blink` uses only the minimal SFR bit it needs: `P2.0`.

Next action:

- Run `mcs51/scripts/probe-toolchain.sh`
- If toolchain is missing, install `sdcc` and `stcgal` or keep `toolchain_pending`
- Once `sdcc` exists, run `mcs51/scripts/build.sh 01_blink`

---

### Phase 1 Toolchain And First Build Verification

Status: phase1_partially_verified / build_verified / hardware_pending.

Completed:

- Marked all `mcs51/scripts/*.sh` executable.
- Ran `mcs51/scripts/probe-toolchain.sh` and confirmed the initial state was real:
  - `sdcc` missing
  - `packihx` missing
  - `stcgal` missing
  - output ended with `toolchain_pending`
- Installed Debian `sdcc` package on the Raspberry Pi:
  - `sudo apt-get install -y sdcc`
- Verified `sdcc` and `packihx` are now available from `/usr/bin`.
- Installed `stcgal` into an isolated repo-local virtual environment:
  - `python3 -m venv mcs51/.venv`
  - `mcs51/.venv/bin/pip install stcgal`
- Updated `mcs51/scripts/probe-toolchain.sh` and `mcs51/scripts/flash.sh` to detect and use `mcs51/.venv/bin/stcgal`.
- Ran the first real build:
  - `mcs51/scripts/build.sh 01_blink`
- Verified generated artifacts under `mcs51/build/01_blink/`:
  - `firmware.ihx`
  - `firmware.hex`
  - SDCC side products such as `.map`, `.mem`, `.lst`, `.sym`

Current state:

- `toolchain_pending`: cleared for the local software toolchain
- `build_pending`: cleared for `01_blink`
- `flash_pending`: script path exists, but not executed against a real serial device
- `hardware_pending`: still true, no connected STC89C52 board/serial bootloader session

Next action:

- Run `mcs51/scripts/check.sh` as the Phase 1 no-hardware gate
- If green, keep Phase 1 scope narrow and start `03_uart_tx`

---

### Phase 1 UART TX Example

Status: phase1_uart_tx_verified / software_ready / hardware_pending.

Completed:

- Added a minimal reusable UART driver layer:
  - `mcs51/common/drivers/uart0.h`
  - `mcs51/common/drivers/uart0.c`
- Expanded the thin SDCC-native register header with only the UART/Timer1 bits needed by the active example:
  - `mcs51/common/core/stc89c52_min.h`
- Added the second active example:
  - `mcs51/examples/03_uart_tx/main.c`
  - `mcs51/examples/03_uart_tx/README.md`
  - `mcs51/examples/03_uart_tx/sources.txt`
- Kept the example scope narrow:
  - fixed string transmit only
  - no receive path
  - no interrupts
  - no ring buffer
  - no CLI
- Locked the example assumptions to the legacy course parameters:
  - `STC89C52RC`
  - `11.0592 MHz`
  - `4800 8N1`
  - `Timer1 mode 2`
  - periodic transmit of `uart ok\r\n`
- Upgraded `mcs51/scripts/build.sh` from single-file compile to:
  - per-source compile into `.rel`
  - final SDCC link into `firmware.ihx`
  - `packihx` generation of `firmware.hex`
- Verified:
  - `mcs51/scripts/build.sh 03_uart_tx`
  - `mcs51/scripts/check.sh`

Artifacts verified for `03_uart_tx`:

- `mcs51/build/03_uart_tx/firmware.ihx`
- `mcs51/build/03_uart_tx/firmware.hex`
- `mcs51/build/03_uart_tx/firmware.map`

Current state:

- `01_blink`: build-verified
- `03_uart_tx`: build-verified
- `04_key_debounce`: not started
- `02_lcd1602`: not started
- hardware validation for both examples remains pending

Next action:

- Keep the line narrow and move to `04_key_debounce`
- Reuse the same shell-based build path and thin-header discipline

---

### Phase 1 Key Debounce And First Host Test

Status: phase1_key_debounce_verified / host_test_ready / hardware_pending.

Completed:

- Added the first pure logic utility module for the 51 line:
  - `mcs51/common/utils/debounce.h`
  - `mcs51/common/utils/debounce.c`
- Added the first host-side test entry for the 51 line:
  - `mcs51/tests/host/test_debounce.c`
  - `mcs51/scripts/test-host.sh`
- Extended `mcs51/scripts/check.sh` so the 51 line now requires host-side logic verification before passing.
- Added `04_key_debounce`:
  - `mcs51/examples/04_key_debounce/main.c`
  - `mcs51/examples/04_key_debounce/README.md`
  - `mcs51/examples/04_key_debounce/sources.txt`
- Kept the scope narrow:
  - single key only
  - polling only
  - no interrupt path
  - no long-press / double-click / repeat
  - outputs only stable `press` and `release` events
- Reused the existing thin-layer approach:
  - example reads raw key level
  - debounce state machine owns stability and edge detection
  - example reacts by toggling LED and emitting simple UART text

Verified:

- `mcs51/scripts/test-host.sh`
- `mcs51/scripts/build.sh 04_key_debounce`
- `mcs51/scripts/check.sh`

Artifacts verified for `04_key_debounce`:

- `mcs51/build/04_key_debounce/firmware.ihx`
- `mcs51/build/04_key_debounce/firmware.hex`
- `mcs51/build/04_key_debounce/firmware.map`

Current state:

- `01_blink`: build-verified
- `03_uart_tx`: build-verified
- `04_key_debounce`: build-verified
- host-side debounce tests: verified
- `02_lcd1602`: not started
- real-board verification remains pending

Next action:

- Decide whether to add a small `mcs51/HARDWARE_PENDING.md` before touching LCD
- Keep LCD for later; do not widen scope until key wiring assumptions are confirmed

---

### Phase 1 Hardware Handoff Boundary

Status: hardware_pending_documented / board_alias_added / gates_green.

Completed:

- Added `mcs51/HARDWARE_PENDING.md` as the 51 line's real-board validation checklist.
- Documented the exact pending boundary:
  - software build is verified for active examples
  - STC serial flashing is not yet run against a real board
  - observed LED/UART/key behavior is not yet verified
- Documented per-example validation expectations for:
  - `01_blink`
  - `03_uart_tx`
  - `04_key_debounce`
- Moved `04_key_debounce` pin assumptions behind board-level aliases:
  - LED alias: `STC89C52_BOARD_LED0`
  - key alias: `STC89C52_BOARD_KEY0_RAW_PRESSED()`
- Updated board documentation to call out the current `P3.1` key/TXD conflict.

Verified after the change:

- `mcs51/scripts/check.sh`
- root STM32 `scripts/check.sh`

Next action:

- Do not start `02_lcd1602` until the board wiring assumptions have either been verified or the `04_key_debounce` UART/key conflict has been resolved by board mapping.
