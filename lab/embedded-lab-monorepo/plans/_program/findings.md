# Migration Findings

## Open Findings

### F005: `planning-with-files` catchup helper path is unavailable in this environment

Evidence:

```bash
$(command -v python3 || command -v python) ~/.codex/skills/planning-with-files/scripts/session-catchup.py /home/qiaopai/Learn_stm32f103
```

Observed result:

```text
/usr/bin/python3: can't open file '/home/qiaopai/.codex/skills/planning-with-files/scripts/session-catchup.py': [Errno 2] No such file or directory
```

Impact:

- Automatic session catchup from the skill package is unavailable on this host.
- Repository-local planning files remain the source of truth for session recovery.

Disposition:

- Continue using `plans/_program/*.md` as the working memory for the task.

### F006: `common/bsp/` is now legacy shared source material, not the active build path

Evidence:

- Active V2 shared code was moved into:
  - `boards/stm32f103c8t6/`
  - `common/core/`
  - `common/drivers/`
  - `common/components/`
  - `common/utils/`
- `scripts/check.sh` passed after verifying example CMake files no longer reference `common/bsp` or raw `BSP` / `STD_PERIPH` lists.

Impact:

- New work should land in layered directories instead of the old mixed BSP bucket.
- Old `common/bsp/` files remain in the repository as inactive compatibility/source material and should not be extended further.

Disposition:

- Treat `common/bsp/` as deprecated for new development.

### F007: No-hardware validation is now stronger than build-only, but runtime truth is still pending

Evidence:

```bash
scripts/check.sh
scripts/probe-stlink.sh
```

Observed result:

- `scripts/check.sh` passed
- `scripts/probe-stlink.sh` returned `Found 0 stlink programmers`

Impact:

- The repository now has a real no-hardware gate:
  - full build
  - warnings as errors
  - artifact checks
  - structure checks
  - CMake contract checks
- This still does not prove board runtime behavior.

Disposition:

- Keep all hardware-facing claims at `hardware_pending` until a real board is flashed and observed.

### F008: Pure logic extraction is now host-testable for CLI and framebuffer paths

Evidence:

```bash
scripts/test-host.sh
```

Observed result:

- `host cli tests passed`
- `host framebuffer tests passed`

Impact:

- Part of the repository can now be verified without an STM32 board:
  - command parsing
  - command dispatch
  - framebuffer drawing primitives
- Future logic extraction should follow the same pattern to keep no-hardware iteration fast.

Disposition:

- Prefer placing hardware-independent behavior in `common/utils/` or similar host-testable layers.

### F009: `ringbuf` is now the reusable byte-stream buffer foundation for CLI input flow

Evidence:

```bash
scripts/test-host.sh
```

Observed result:

- `host ringbuf tests passed`

Impact:

- Byte-oriented buffering is no longer implemented ad hoc inside `07_usart_cli`
- Future UART logging, RX buffering, and line-based command ingestion can reuse one tested core module

Disposition:

- Keep transport-agnostic buffering in `common/utils/ringbuf.[ch]`

### F010: `03_exti_key` now uses EXTI + debounce layering instead of direct ISR-side LED behavior

Evidence:

- `examples/03_exti_key/main.c` now increments `g_exti_count` in `EXTI0_IRQHandler`
- LED toggling moved to main-loop logic driven by `debounce_pressed_event()`
- `scripts/check.sh` passed after the refactor

Impact:

- The key example is now closer to maintainable embedded structure:
  - ISR does minimal bookkeeping
  - behavior is handled outside the interrupt path
  - debounce logic is reusable and host-testable

Disposition:

- Reuse the same layering for future input examples where practical

### F011: Debounce behavior is logic-verified on host, but threshold tuning remains hardware-pending

Evidence:

```bash
scripts/test-host.sh
scripts/probe-stlink.sh
```

Observed result:

- `host debounce tests passed`
- `Found 0 stlink programmers`

Impact:

- State-machine correctness is covered by host-side tests
- Real button bounce characteristics may still require threshold adjustment on the actual board

Disposition:

- Keep debounce runtime claims at `hardware_pending` until the example is flashed and observed on real hardware

### F012: The repository now has a release-facing handoff surface, not only development notes

Evidence:

- Added:
  - `CHANGELOG.md`
  - `RELEASE_NOTES.md`
  - `HARDWARE_PENDING.md`
  - `COMPATIBILITY.md`

Impact:

- The repo now exposes a clearer delivery boundary:
  - what is done
  - what remains pending
  - what environment is expected
  - what this version explicitly does not claim

Disposition:

- Keep these files aligned with future repository milestones instead of leaving release state implicit in progress logs

### F013: Hardware validation is now transferable through generated validation bundles

Evidence:

- Added `scripts/package-validation-bundle.sh`
- `scripts/check.sh` now verifies bundle generation and contents

Impact:

- `hardware_pending` is no longer just a blocked local state
- Another person can take the packaged artifacts, README, flash command, and metadata and perform board-side validation independently

Disposition:

- Use the bundle as the default handoff artifact for any bench-side validation request

### F014: Host-side `cli_input_pump` testing caught a real incremental-processing bug

Evidence:

```bash
scripts/check.sh
```

Observed result before fix:

```text
==> Host test: CLI input pump
line matches: expected 1, got 0
```

Root cause:

- `cli_input_pump_process()` stored line length in a local variable.

### F015: `/home/qiaopai/Learn_stm32f103` is now the correct monorepo root

Evidence:

- It is the only tracked git repository among the active workspaces
- `mcs51/` already lives under it
- `rpi_hpad/` was previously outside git and can be imported cleanly as a sibling project

Impact:

- The correct merge strategy is not "make three repos talk to each other"
- The correct merge strategy is "one git repo, three subprojects, three independent build systems"

Disposition:

- Treat the repository as a monorepo from this point onward
- Keep STM32, MCS-51, and RPi HPAD isolated at the build/runtime layer and unified only at the git/documentation/top-level gate layer
- The firmware calls the pump incrementally as UART bytes arrive, so previously read characters were discarded before newline handling.

Fix:

- Added persistent `line_length` to `cli_input_pump_t`.
- Reset line length only after a full line is delivered or an overflow is detected.

Impact:

- This confirms the value of host-side tests: a bug that would have looked like a UART/hardware issue was caught without a board.

Disposition:

- Keep `cli_input_pump` in the host-side gate.

### F015: The repository now has an explicit software-freeze marker independent of git tagging

Evidence:

- Added `VERSION` with `v2.0.0-soft-validated`
- Added `REALBOARD_VALIDATION_CHECKLIST.md`

Impact:

- The repository can be handed off as a frozen software milestone even before a formal git tag is created
- Hardware closure work is now framed as an execution checklist, not as an ambiguous future task

Disposition:

- When VCS tagging is desired, tag this frozen state instead of continuing to blur the milestone boundary

### F016: `STC89C52` was added as source material, not as a migrated build target

Evidence:

- Imported upstream `Learn_STC89C52` into `mcs51/Learn_STC89C52/`
- Added `mcs51/README.md` and `mcs51/UPSTREAM.md`

Impact:

- The repository now spans two learning tracks:
  - STM32F103 bare-metal mainline
  - MCS-51 / STC89C52 source material
- This does not imply shared build, flash, or test infrastructure across the two MCU families

Disposition:

- Keep 8051 migration work as a separate future track with its own tooling assumptions and verification path

### F001: ST-Link not currently detected

Evidence:

```bash
st-info --probe
```

Observed result:

```text
Found 0 stlink programmers
```

Impact:

- Build migration can proceed.
- Actual flash verification cannot be marked complete until ST-Link is connected and detected.

Disposition:

- Keep flash validation as `hardware_pending` until physical connection is available.

### F002: Legacy extracted chapter directories contain empty placeholder files

Evidence:

- Workspace copy of `03GPIO通用输入输出口/3-1 LED闪烁/` contains many `0`-byte files, including `User/main.c`, `System/Delay.c`, and startup files.

Impact:

- The checked-out directory tree cannot be trusted as the only migration source.
- Migration had to pivot to:
  - `02软件安装及创建工程/2-2 库函数方式/` for shared CMSIS + StdPeriph
  - `pi_gcc_f103_blink/` for OLED GNU seed code

Disposition:

- Keep legacy directories untouched.
- Treat decompressed chapter directories in the repo as incomplete source material.

### F003: Current `7z` can list RAR5 course archives but cannot extract them

Evidence:

- `7z l` works on chapter `.rar` files
- `7z x` fails with repeated `ERROR: Unsupported Method`

Impact:

- Direct one-to-one migration from compressed chapter examples is blocked in the current Raspberry Pi environment.
- Current examples are semantically migrated and organized for bare-metal GCC/CMake, but some chapter-specific runtime logic had to be rebuilt instead of copied from the `.rar` contents.

Disposition:

- Record as tooling gap, not as migration success.
- Future exact-source migration requires installing a RAR5-capable extractor such as `unrar`.

### F004: Hardware-facing pin maps are compile-verified but not board-verified

Evidence:

- `scripts/build-all.sh` passed for all examples.
- No ST-Link probe is available to flash and observe runtime behavior.

Impact:

- The following mappings are documented but remain hardware-pending:
  - `PC13` LED
  - `PA0` EXTI / ADC / TIM2_CH1 reuse
  - `PB6/PB7` hardware I2C
  - `PB8/PB9` software I2C OLED
  - `PA9/PA10` USART1
  - `PA4/PA5/PA6/PA7` SPI1

Disposition:

- Keep all flash/runtime claims at `hardware_pending` until real-board verification is completed.
