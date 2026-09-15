# STC89C52 Migration File Map

## Planning Files

- `plans/mcs51_migration/task_plan.md`
- `plans/mcs51_migration/findings.md`
- `plans/mcs51_migration/progress.md`
- `plans/mcs51_migration/files.md`

## Source Material

- `mcs51/Learn_STC89C52/`
- `mcs51/UPSTREAM.md`
- `mcs51/README.md`

## Planned Active 51 Layout

- `mcs51/boards/stc89c52rc/`
- `mcs51/common/core/`
- `mcs51/common/drivers/`
- `mcs51/common/components/`
- `mcs51/common/utils/`
- `mcs51/examples/01_blink/`
- `mcs51/examples/02_lcd1602/`
- `mcs51/examples/03_uart_tx/`
- `mcs51/examples/04_key_debounce/`
- `mcs51/scripts/build.sh`
- `mcs51/scripts/probe-toolchain.sh`
- `mcs51/scripts/build-all.sh`
- `mcs51/scripts/check.sh`
- `mcs51/scripts/flash.sh`
- `mcs51/scripts/package-validation-bundle.sh`

## Implemented Phase 1 Files

- `mcs51/scripts/probe-toolchain.sh`
- `mcs51/scripts/build.sh`
- `mcs51/scripts/build-all.sh`
- `mcs51/scripts/check.sh`
- `mcs51/scripts/flash.sh`
- `mcs51/boards/stc89c52rc/README.md`
- `mcs51/boards/stc89c52rc/board.h`
- `mcs51/common/core/stc89c52_min.h`
- `mcs51/examples/01_blink/README.md`
- `mcs51/examples/01_blink/main.c`

## Keep Separate From STM32

- `boards/stm32f103c8t6/`
- `common/`
- `examples/`
- `cmake/stm32f103_example.cmake`
- `scripts/check.sh`
