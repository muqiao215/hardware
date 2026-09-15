# STM32F103 Bare-Metal Migration Plan

## Goal

Convert this repository from Keil/IDE-centered course material into a Raspberry Pi friendly bare-metal STM32F103 lab repository.

The end state is not only "can compile one demo". The end state is:

- Every migrated experiment builds with GNU Arm Embedded + CMake + Ninja.
- Every runnable firmware produces `.elf`, `.hex`, and `.bin`.
- Flashing is available from the Raspberry Pi through ST-Link.
- Legacy Keil/course files remain available as source material, but no longer define the primary workflow.
- The repository has a repeatable build/flash/check workflow that can be used without a visual IDE.

## Current State

- Main bare-metal seed project: `pi_gcc_f103_blink/`
- Current verified build command:

```bash
cmake -S pi_gcc_f103_blink -B /tmp/pi_gcc_f103_blink-check -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=pi_gcc_f103_blink/cmake/toolchains/arm-gcc.cmake
cmake --build /tmp/pi_gcc_f103_blink-check
```

- Current flash entry:

```bash
cmake --build pi_gcc_f103_blink/build --target flash
```

- Legacy source material:

```text
02软件安装及创建工程/
03GPIO通用输入输出口/
04OLED调试工具/
05EXIT外部中断/
06TIM定时器/
07ADC模数转换器/
08DMA直接存储器读取/
09USART串口/
10IIC通信/
11SPI通信/
```

## Migration Policy

Do not destructively rewrite legacy course folders in this migration. Treat them as source material.

New bare-metal code should land under a modern layout:

```text
examples/
  01_gpio_led/
  02_oled_i2c/
  03_exti_key/
  04_tim_pwm/
  05_adc_polling/
  06_adc_dma/
  07_usart_cli/
  08_i2c_oled/
  09_spi_loopback/
common/
  cmsis/
  stdperiph/
  bsp/
boards/
  stm32f103c8t6/
cmake/
  toolchains/
scripts/
  build-all.sh
  flash.sh
  probe-stlink.sh
```

The existing `pi_gcc_f103_blink/` can either remain as the first migrated example or be folded into `examples/02_oled_i2c/` after the shared layout is ready.

## Scope

### Must Migrate

- GPIO LED blink and GPIO output basics.
- OLED SSD1306 display demo.
- Software I2C or hardware I2C OLED path, depending on what the current code supports cleanly.
- EXTI key input demo.
- Timer delay/PWM demo.
- ADC polling demo.
- DMA-backed ADC demo if the legacy material contains enough complete code.
- USART serial demo.
- SPI demo.
- Shared startup, linker script, CMSIS, StdPeriph, delay, and board definitions.
- Build scripts and one-command all-example build.
- ST-Link probe and flash scripts.

### May Defer With Reason

- Hardware features that cannot be verified without missing peripherals.
- Course-only notes that are not executable firmware.
- Examples that require unclear pin wiring until wiring documentation is written.

## Phases

## V2 Refactor Goal

After the first migration pass, the primary goal is no longer "add more examples".

The new V2 goal is:

- Separate board, core, driver, component, and utility boundaries clearly.
- Remove per-example hand-maintained source file lists from CMake.
- Turn examples into thin assembly layers around reusable modules.
- Add a stronger no-hardware quality gate beyond "can compile".
- Prioritize reusable extraction for:
  - USART CLI
  - SSD1306 + framebuffer + bus abstraction

Target layout for active code:

```text
boards/
  stm32f103c8t6/
common/
  core/
  drivers/
  components/
  utils/
  cmsis/
  stdperiph/
examples/
  01_gpio_led/
  ...
  09_spi_loopback/
cmake/
scripts/
```

### Phase 6: V2 Boundary Refactor

- Move active board-facing code from `common/bsp/` into:
  - `boards/stm32f103c8t6/`
  - `common/core/`
  - `common/drivers/`
  - `common/components/`
  - `common/utils/`
- Keep `common/bsp/` out of the active build path.
- Preserve legacy material and avoid destructive cleanup.

Exit gate:

- No example CMake file manually lists `common/bsp/*.c`
- Active shared code is organized by responsibility instead of a mixed BSP bucket

Status:

- Complete for the first V2 pass
- Active build path now uses `boards/`, `common/core/`, `common/drivers/`, `common/components/`, and `common/utils/`

### Phase 7: V2 Unified Example CMake

- Replace raw `BSP` / `STD_PERIPH` source enumeration with a module-based API.
- Each example should declare:
  - board
  - main source
  - required modules
- Common helper should own:
  - startup/core sources
  - include paths
  - warnings-as-errors
  - map / hex / bin generation

Exit gate:

- All example `CMakeLists.txt` files use the same `BOARD + MODULES` style helper
- No example CMake file enumerates StdPeriph source files directly

Status:

- Complete for the first V2 pass
- Example CMake files now declare `BOARD` + `MODULES` instead of enumerating raw BSP/StdPeriph sources

### Phase 8: V2 Reusable Modules + Check Gate

- Extract reusable USART CLI layers:
  - parser
  - dispatch
  - port
- Extract reusable OLED path:
  - framebuffer
  - SSD1306 component
  - bus port abstraction
- Add `scripts/check.sh` as the main no-hardware gate

Exit gate:

```bash
scripts/check.sh
```

Status:

- Complete for the first V2 pass
- Reusable `cli_parser`, `cli_dispatch`, `usart_cli_port`, `framebuffer`, `ssd1306`, and `soft_i2c_bus` modules are in place
- `scripts/check.sh` is now the main no-hardware gate

### Phase 1: Normalize Build Skeleton

Create shared infrastructure:

- Root or shared CMake organization.
- Board definition for STM32F103C8T6.
- Toolchain file.
- Linker script.
- Startup file.
- Shared CMSIS and StdPeriph dependency path.
- Common flash target convention.

Exit gate:

```bash
cmake -S examples/01_gpio_led -B /tmp/stm32-gpio-led -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake
cmake --build /tmp/stm32-gpio-led
```

Status:

- Complete
- Shared `common/`, `boards/`, `cmake/` and reusable example helper are in place

### Phase 2: Migrate Minimal GPIO Example

Build the smallest firmware first:

- Reset/startup works.
- System clock works.
- GPIO output works.
- LED blink runs on the target board pin documented by the board file.

Exit gate:

- `.elf`, `.hex`, `.bin` generated.
- `arm-none-eabi-size` prints firmware size.
- `cmake --build <build-dir> --target flash` is wired.

Status:

- Complete
- `examples/01_gpio_led` builds and exposes a `flash` target

### Phase 3: Migrate OLED/I2C Demo

Promote the current `pi_gcc_f103_blink` OLED animation into the normalized layout.

Exit gate:

- OLED source builds from the new shared layout.
- Pin mapping is documented.
- Firmware binary size remains inside STM32F103C8T6 flash/RAM limits.

Status:

- Complete
- `pi_gcc_f103_blink/` logic has been normalized into `examples/02_oled_i2c`

### Phase 4: Migrate Input, Timer, ADC, DMA, USART, SPI

Migrate each peripheral as an independent example:

- `03_exti_key`
- `04_tim_pwm`
- `05_adc_polling`
- `06_adc_dma`
- `07_usart_cli`
- `09_spi_loopback`

Each example must have:

- Its own README.
- Pin table.
- Build command.
- Flash command.
- Expected runtime behavior.

Exit gate:

```bash
scripts/build-all.sh
```

Status:

- Complete for buildable GCC examples
- Implemented:
  - `03_exti_key`
  - `04_tim_pwm`
  - `05_adc_polling`
  - `06_adc_dma`
  - `07_usart_cli`
  - `08_i2c_hw`
  - `09_spi_loopback`
- Note: exact one-to-one source extraction from `.rar` archives is still blocked by the current extractor tooling

### Phase 5: Raspberry Pi Flash Workflow

Add scripts:

- `scripts/probe-stlink.sh`
- `scripts/flash.sh <example-name>`
- Optional OpenOCD config path if `st-flash` is insufficient.

Exit gate:

```bash
scripts/probe-stlink.sh
scripts/flash.sh 01_gpio_led
```

Status:

- Partially complete
- Scripts exist and are runnable
- Hardware verification is pending because no ST-Link is detected

### Phase 6: Documentation Cutover

Update root README so the primary workflow is:

1. Install dependencies on Raspberry Pi.
2. Build one example.
3. Probe ST-Link.
4. Flash firmware.
5. Verify expected behavior.
6. Build all examples.

Legacy folders remain documented as source material only.

Status:

- Complete
- Root README now presents the Raspberry Pi bare-metal workflow as primary

## Global Gates

Run these before declaring the migration complete:

```bash
scripts/build-all.sh
```

```bash
find examples -name README.md -print
```

```bash
scripts/probe-stlink.sh
```

If ST-Link is physically unavailable, record that as `hardware_pending` in `plans/_program/progress.md`, not as a successful flash.

Current completion state:

- Build/documentation migration: complete
- Hardware flash/runtime verification: pending on physical ST-Link / board connection

## Stoplines

Stop and record a finding if any of these happen:

- Legacy code depends on Keil-specific compiler extensions that are not mechanically portable.
- A peripheral example requires unknown wiring.
- The code builds but exceeds STM32F103C8T6 memory limits.
- ST-Link is not detected by `st-info --probe`.
- Flash succeeds but runtime behavior cannot be observed remotely.

## Completion Definition

The migration is complete when:

- All target examples build with GNU Arm Embedded.
- `scripts/build-all.sh` passes.
- At least one example has been flashed through ST-Link from the Raspberry Pi.
- Hardware-unverified examples are explicitly marked with reason.
- Root README no longer presents Keil as the primary workflow.
