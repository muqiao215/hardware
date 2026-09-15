# Migration Progress

## 2026-04-11

### V2 refactor session

Goal:

- Stop expanding example count.
- Tighten active code boundaries around `boards/`, `common/core/`, `common/drivers/`, `common/components/`, and `common/utils/`.
- Replace per-example source enumeration with a module-based CMake helper.
- Add `scripts/check.sh` as the main no-hardware gate.
- First reusable extraction targets:
  - `07_usart_cli`
  - `02_oled_i2c`

Initial observation:

- Existing v1 migration builds all 9 examples.
- `common/bsp/` is a mixed bucket containing board APIs, delay, soft I2C, SSD1306, OLED drawing, and interrupt templates.
- Example `CMakeLists.txt` files still hand-list `BSP` and `STD_PERIPH` sources, which makes examples too thick and maintenance-heavy.
- `planning-with-files` session catchup helper was not present at `~/.codex/skills/planning-with-files/scripts/session-catchup.py` in this environment; proceeding from repository plan files instead.

Next action:

- Implement V2 boundary split and module-based CMake helper.

---

### V2 refactor result

Status: v2_structure_verified / hardware_pending.

Completed in this session:

- Active shared code moved into explicit layers:
  - `boards/stm32f103c8t6/board.[ch]`
  - `common/core/interrupts.[ch]`
  - `common/core/stm32f10x_conf.h`
  - `common/drivers/`
  - `common/components/`
  - `common/utils/`
- `cmake/stm32f103_example.cmake` rewritten around `BOARD + MODULES`.
- All example `CMakeLists.txt` files no longer enumerate raw `BSP` / `STD_PERIPH` source lists.
- `07_usart_cli` upgraded from simple echo to reusable CLI structure:
  - `cli_parser`
  - `cli_dispatch`
  - `usart_cli_port`
- `02_oled_i2c` upgraded to reusable display stack:
  - `framebuffer`
  - `ssd1306`
  - `soft_i2c_bus`
- `04_tim_pwm`, `08_i2c_hw`, and `09_spi_loopback` now use reusable driver entry points.
- `scripts/build-all.sh` now auto-discovers examples.
- `scripts/check.sh` added as the first-class no-hardware validation gate.
- Root `README.md` updated to document the V2 structure and validation workflow.
- Added host-side logic tests:
  - `tests/host/test_cli.c`
  - `tests/host/test_framebuffer.c`
  - `scripts/test-host.sh`
- Example READMEs were normalized toward handoff/verification content:
  - wiring
  - expected behavior
  - artifact paths
  - common failure reasons

Verification evidence:

```bash
scripts/check.sh
```

Observed result:

- All 9 examples built successfully with `-Werror`.
- Host-side CLI and framebuffer tests passed on the Raspberry Pi.
- All example build directories contained:
  - `firmware.elf`
  - `firmware.hex`
  - `firmware.bin`
  - `firmware.map`
- Structure and CMake contract checks passed.
- Size summary printed for all examples.

```bash
scripts/probe-stlink.sh
```

Observed result:

```text
Found 0 stlink programmers
```

Hardware status:

- `hardware_pending`
- Build and no-hardware engineering gates are complete for this V2 pass.
- Flash and runtime observation are still blocked by missing ST-Link connection.

Next action:

- When hardware is connected, re-run:
  - `scripts/probe-stlink.sh`
  - `scripts/flash.sh 01_gpio_led`
  - `scripts/flash.sh 02_oled_i2c`
  - `scripts/flash.sh 07_usart_cli`
- Validate CLI behavior, OLED refresh, and the documented pin map on the real board.

---

### V2 logic extraction pass

Status: v2_logic_extracted / hardware_pending.

Completed in this session:

- Added reusable pure-logic modules:
  - `common/utils/ringbuf.[ch]`
  - `common/utils/debounce.[ch]`
- Added CLI assembly layer:
  - `common/cli/cli_input_pump.[ch]`
- Slimmed `07_usart_cli`:
  - removed local hand-managed line buffering
  - now assembles `cli_input_pump + ringbuf + cli_parser + cli_dispatch + usart_cli_port`
- Slimmed `03_exti_key`:
  - `EXTI0_IRQHandler` now only records interrupt activity
  - LED toggling moved behind reusable `debounce` state handling in the main loop
- Added host-side verification:
  - `tests/host/test_ringbuf.c`
  - `tests/host/test_debounce.c`
- Extended `scripts/test-host.sh` and `scripts/check.sh` to include the new logic gates.

Verification evidence:

```bash
scripts/test-host.sh
```

Observed result:

- `host cli tests passed`
- `host ringbuf tests passed`
- `host debounce tests passed`
- `host framebuffer tests passed`
- `test-host.sh: all host-side tests passed`

```bash
scripts/check.sh
```

Observed result:

- All 9 examples built successfully with `-Werror`
- Host-side CLI, ringbuf, debounce, and framebuffer tests passed
- Artifact, structure, and CMake contract checks passed
- Size summary printed for all examples

```bash
scripts/probe-stlink.sh
```

Observed result:

```text
Found 0 stlink programmers
```

Hardware status:

- `hardware_pending`

Next action:

- When hardware is connected, re-run:
  - `scripts/probe-stlink.sh`
  - `scripts/flash.sh 03_exti_key`
  - `scripts/flash.sh 07_usart_cli`
- Tune debounce threshold against the real button and confirm CLI interaction on the board.

---

### V2 release packaging pass

Status: v2_release_packaged / hardware_pending.

Completed in this session:

- Added release-facing documentation:
  - `CHANGELOG.md`
  - `RELEASE_NOTES.md`
  - `HARDWARE_PENDING.md`
  - `COMPATIBILITY.md`
- Added handoff packaging entry point:
  - `scripts/package-validation-bundle.sh`
- Strengthened host-side tests:
  - added `tests/host/test_cli_input_pump.c`
  - expanded ringbuf edge coverage
  - expanded debounce edge coverage
  - expanded CLI parser edge coverage
- Extended `scripts/check.sh` so the no-hardware gate now also verifies:
  - release-surface files exist
  - validation bundles can be generated
  - each bundle contains firmware artifacts plus handoff metadata

Verification evidence:

```bash
scripts/check.sh
```

Observed result:

- full build passes
- host-side CLI, cli_input_pump, ringbuf, debounce, and framebuffer tests pass
- release files exist
- validation bundle generation succeeds
- `07_usart_cli` rebuilt after fixing persistent line-state handling in `cli_input_pump`

---

### Monorepo consolidation pass

Status: complete / monorepo_soft_validated / hardware_pending.

Goal:

- Stop treating STM32, MCS-51, and Raspberry Pi HPAD as separate loose workspaces.
- Consolidate them into one monorepo with one root README and one aggregate gate.
- Preserve each subproject's own build system and hardware boundary.

Completed:

- Selected `/home/qiaopai/Learn_stm32f103` as the monorepo root because it is already the tracked git repository with `origin`
- Confirmed `mcs51/` is already inside the main repository
- Imported `rpi_hpad/` into the main repository as a sibling project
- Added root aggregate gate:
  - `scripts/check-all.sh`
- Rewrote root `README.md` as a monorepo entry for:
  - STM32F103
  - MCS-51 / STC89C52
  - Raspberry Pi HPAD
- Extended root `.gitignore` for Python package and venv artifacts from `rpi_hpad/`

Verification evidence:

```bash
mcs51/scripts/check.sh
rpi_hpad/scripts/check.sh
scripts/check-all.sh
```

Observed result:

- MCS-51 no-hardware gate passed
- RPi HPAD no-hardware gate passed
- Root aggregate gate passed across STM32F103, MCS-51, and RPi HPAD

Hardware status:

- `hardware_pending`

Next action:

- Keep future hardware claims scoped to the corresponding subproject until real boards are connected
- Re-run only the affected subproject gate when changing one line, and re-run `scripts/check-all.sh` before the next cross-project freeze

---

### V2 version freeze pass

Status: v2_soft_validated / hardware_pending.

Completed in this session:

- Declared repository freeze version:
  - `VERSION` = `v2.0.0-soft-validated`
- Added explicit real-board execution checklist:
  - `REALBOARD_VALIDATION_CHECKLIST.md`
- Updated release-facing docs to state the frozen position clearly:
  - software closure complete
  - hardware closure pending real execution

Verification evidence:

```bash
scripts/check.sh
```

Observed result:

- no-hardware gate still passes after adding the freeze/version surface

```bash
scripts/probe-stlink.sh
```

Observed result:

```text
Found 0 stlink programmers
```

Next action:

- Create a git tag from this commit when version history should be frozen in VCS
- Execute `REALBOARD_VALIDATION_CHECKLIST.md` on a hardware bench once ST-Link is available

---

### MCS-51 source-material import pass

Status: mcs51_source_material_imported / stm32_v2_preserved.

Completed in this session:

- Imported upstream `Learn_STC89C52` into:
  - `mcs51/Learn_STC89C52/`
- Added MCS-51 track docs:
  - `mcs51/README.md`
  - `mcs51/UPSTREAM.md`
- Updated root docs so the repo now clearly distinguishes:
  - STM32F103 bare-metal mainline
  - STC89C52 source-material track

Boundary kept intentionally:

- No attempt was made to force `STC89C52` into the existing STM32 CMake/Ninja flow
- `scripts/check.sh` and validation bundles remain STM32-scoped

Next action:

- If needed later, create a separate migration plan for `STC89C52` instead of mixing 8051 assumptions into the STM32 platform layer

---

Status: migrated_build_verified / hardware_pending.

Facts:

- `pi_gcc_f103_blink/` exists as the current bare-metal seed project.
- It already builds into `firmware.elf`, `firmware.hex`, and `firmware.bin`.
- It has a CMake `flash` target using `st-flash --reset write`.
- Current `st-info --probe` reports no connected ST-Link programmer, so hardware flash is not currently verified.
- Legacy Keil/course material still exists under the original chapter folders.
- New normalized layout now exists:
  - `common/`
  - `boards/stm32f103c8t6/`
  - `cmake/`
  - `examples/`
  - `scripts/build-all.sh`
  - `scripts/probe-stlink.sh`
  - `scripts/flash.sh`
- Migrated examples now exist and build independently:
  - `01_gpio_led`
  - `02_oled_i2c`
  - `03_exti_key`
  - `04_tim_pwm`
  - `05_adc_polling`
  - `06_adc_dma`
  - `07_usart_cli`
  - `08_i2c_hw`
  - `09_spi_loopback`
- Shared build helper `cmake/stm32f103_example.cmake` produces `.elf/.hex/.bin` and a `flash` target for each example.

Verification evidence:

```bash
scripts/build-all.sh
```

Observed result:

- All 9 examples configured and built successfully with GNU Arm Embedded + CMake + Ninja.
- Each build directory under `build/<example>/` now contains `firmware.elf`, `firmware.hex`, and `firmware.bin`.

```bash
find examples -maxdepth 2 -name README.md -print | sort
```

Observed result:

- All 9 example directories contain a local `README.md`.

```bash
scripts/probe-stlink.sh
```

Observed result:

```text
Found 0 stlink programmers
```

Hardware status:

- `hardware_pending`
- Flash/board runtime verification remains blocked by missing ST-Link connection in the Raspberry Pi environment.

Next action:

- When hardware is connected, run:
  - `scripts/probe-stlink.sh`
  - `scripts/flash.sh 01_gpio_led`
  - `scripts/flash.sh 02_oled_i2c`
- Validate board-specific wiring for `PA0`, `PB6/PB7`, `PB8/PB9`, `PA4/5/6/7`, `PA9/PA10` against the real board and peripherals.
