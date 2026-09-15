# STC89C52 Migration Findings

## F001: 51 source material is Keil/C51-oriented, not immediately SDCC-ready

Evidence:

- Example source includes:

```c
#include <REGX52.H>
```

Impact:

- Direct Linux CLI compilation with SDCC may require a compatibility layer or source rewrite.
- `REGX52.H`, `sbit`, `sfr`, interrupt syntax, and memory qualifiers must be handled deliberately.

Disposition:

- Start with a tiny SDCC-native blink example before migrating all lessons.
- Add compatibility rules only after seeing concrete compile errors.

## F002: Required 8051 toolchain is not currently installed on PATH

Evidence:

```bash
command -v sdcc
command -v packihx
command -v stcgal
```

Observed:

- `python3` exists
- `sdcc` not found
- `packihx` not found
- `stcgal` not found

Impact:

- 51 build migration cannot be truthfully marked build-verified yet.

Disposition:

- First implementation phase should add a probe script and either install tooling or mark `toolchain_pending`.

## F003: STC89C52 flashing is a different hardware workflow from STM32/ST-Link

Evidence:

- STM32 path uses `st-flash` / ST-Link.
- STC89C52 normally uses a UART bootloader path such as `stcgal`.

Impact:

- STM32 `scripts/flash.sh` should not be reused for 51.
- 51 hardware validation needs serial port, reset/power-cycle procedure, and board-specific wiring notes.

Disposition:

- Add `mcs51/scripts/flash.sh` only after toolchain and artifact format are established.
- Keep 51 hardware validation as `hardware_pending` until real board execution.

## F004: Upstream lesson directories include both loose source files and zip archives

Evidence:

- Multiple lesson subdirectories contain `.zip` files.
- Many also contain `.c/.h` files directly.

Impact:

- Some examples may already be extractable from loose files.
- Others may need zip inspection or extraction.

Disposition:

- For the first phase, prefer loose source files and simple rewritten examples.
- Treat zip archives as source material to inspect later, not as active build inputs.

## F005: Phase 1 should use shell scripts, not CMake

Evidence:

- The 51 line still has unresolved toolchain and compiler-syntax risks.
- SDCC is not installed yet on the Raspberry Pi.

Impact:

- Adding CMake now would mix build-system risk with compiler/header risk.

Disposition:

- Use `mcs51/scripts/*.sh` first.
- Revisit CMake only after `01_blink` and `03_uart_tx` are stable.

## F006: Thin SDCC-native headers are safer than a broad Keil compatibility layer

Evidence:

- Upstream examples use Keil-style `REGX52.H`.
- The first active example only needs one pin bit: `P2.0`.

Impact:

- A broad fake `REGX52.H` could hide unsupported syntax and create false confidence.

Disposition:

- Added `mcs51/common/core/stc89c52_min.h` with only the definitions needed by active examples.
- Add SFR/sbit declarations only when a migrated example actually needs them.

## F007: Debian Bookworm/Trixie Python packaging blocks direct `pip install --user` for `stcgal`

Evidence:

- `python3 -m pip install --user stcgal` failed with:

```text
error: externally-managed-environment
```

Impact:

- Earlier install guidance using plain `pip install --user` is not reliable on this Raspberry Pi.

Disposition:

- Use an isolated repo-local virtual environment instead:
  - `python3 -m venv mcs51/.venv`
  - `mcs51/.venv/bin/pip install stcgal`
- Update `probe-toolchain.sh` and `flash.sh` to look for `mcs51/.venv/bin/stcgal`.

## F008: `packihx` does not support `--version`

Evidence:

- Running `packihx --version` returned:

```text
packihx: cannot open --version
```

Impact:

- A naive version probe makes the tool look broken even though it is installed and usable.

Disposition:

- Treat `packihx` as a presence-only tool in `probe-toolchain.sh`.

## F009: 51 Phase 1 is now build-proven, not only scaffolded

Evidence:

- `mcs51/scripts/build.sh 01_blink` succeeded.
- `packihx` reported:

```text
packihx: read 17 lines, wrote 23: OK.
```

- Artifacts created:
  - `mcs51/build/01_blink/firmware.ihx`
  - `mcs51/build/01_blink/firmware.hex`
  - `mcs51/build/01_blink/firmware.map`

Impact:

- The 51 line has moved from pure planning/scaffold state into a real minimal software build baseline.

Disposition:

- Keep scope narrow.
- Use this proven `01_blink` chain as the base for `03_uart_tx`.

## F010: SDCC multi-file builds need explicit compile-then-link handling

Evidence:

- Attempting to pass multiple `.c` files directly to SDCC produced:

```text
warning 120: cannot compile more than one source file
```

- Link then failed with undefined UART symbols.

Impact:

- A GCC-style single command over multiple C translation units is not reliable here.
- The 51 build script must manage intermediate `.rel` objects explicitly once examples stop being single-file.

Disposition:

- Updated `mcs51/scripts/build.sh` to:
  - compile each source into its own `.rel`
  - link the `.rel` files into `firmware.ihx`
  - then run `packihx`

## F011: `03_uart_tx` should stay fixed-parameter and transmission-only in Phase 1

Evidence:

- The legacy lesson already fixes a clear UART baseline:
  - `4800bps`
  - `11.0592MHz`
  - Timer1 baud-rate generation
- The current migration goal is to create an observation port, not a full serial stack.

Impact:

- Adding receive, ISR, ring buffers, or CLI now would mix too many variables at the moment the 51 line has just become build-stable.

Disposition:

- `03_uart_tx` only sends `uart ok\r\n` periodically.
- README now pins down:
  - clock
  - baud rate
  - TX pin
  - expected terminal output
  - first troubleshooting checks

## F012: `04_key_debounce` is the first 51 module that benefits from host-side testing

Evidence:

- The debounce state machine depends only on sampled input levels.
- It does not require SFR access, timing peripherals, or STC flashing.

Impact:

- This is the right first place to prove that the 51 line can reuse the STM32 v2 pattern of "logic off-board, hardware on-board".

Disposition:

- Added:
  - `mcs51/common/utils/debounce.*`
  - `mcs51/tests/host/test_debounce.c`
  - `mcs51/scripts/test-host.sh`
- Extended `mcs51/scripts/check.sh` so host-side debounce tests are now part of the 51 no-hardware gate.

## F013: The current `04_key_debounce` pin choice conflicts with UART0 TX on classic 8051 pin mapping

Evidence:

- `P3.1` is commonly `TXD` on 8051/STC89C52 devices.
- The example currently uses:
  - `P3.1` as the raw key input
  - UART output for `key press` / `key release`

Impact:

- The software build is valid, but real-board verification may fail or become ambiguous if the actual learning board also wires a key to `P3.1`.
- Without checking the board schematic or silkscreen wiring, this cannot be claimed as hardware-ready.

Disposition:

- Documented the conflict clearly in `mcs51/examples/04_key_debounce/README.md`.
- Keep hardware status as pending.
- Before real-board validation, confirm the actual key pin and either:
  - move the key to a non-UART pin, or
  - disable UART output and validate only via LED behavior.

## F014: Board-level aliases are the right minimum abstraction for the 51 hardware boundary

Evidence:

- `04_key_debounce` originally read `P3.1` directly in example code.
- The real uncertainty is not debounce logic, but the board's actual LED/KEY/UART wiring.

Impact:

- If pin assumptions stay inside the example body, every real-board correction turns into example surgery.
- A thin board alias lets the line stay simple without prematurely building a full BSP.

Disposition:

- Added board-level aliases in `mcs51/boards/stc89c52rc/board.h`.
- Updated `04_key_debounce` to consume board aliases instead of raw pin names.
- Kept the abstraction deliberately thin:
  - enough to relocate pins
  - not yet a full driver or board-init layer
