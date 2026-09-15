# Real Board Validation Checklist

Current target version: `v2.0.0-soft-validated`

Current software status:

- software-side closure complete
- hardware-side closure pending ST-Link access

## Preconditions

- Blue Pill style `STM32F103C8T6` board
- Working ST-Link
- USB serial adapter for `07_usart_cli`
- OLED module for `02_oled_i2c`
- Target host with this repository checkout

## Step 1: Connect And Probe

1. Connect ST-Link to the board
2. Run:

```bash
scripts/probe-stlink.sh
```

Pass:

- at least one ST-Link programmer is detected

Fail:

- output is still `Found 0 stlink programmers`

First checks on failure:

- ST-Link USB cable and power
- SWDIO / SWCLK / GND / 3V3 wiring
- board power state
- USB permission / host visibility

## Step 2: Smoke Flash

Run these in order:

```bash
scripts/flash.sh 01_gpio_led
scripts/flash.sh 02_oled_i2c
scripts/flash.sh 07_usart_cli
```

Pass:

- all three examples flash without `st-flash` failure

Fail:

- flash command errors
- target is not detected
- write / reset fails

First checks on failure:

- rerun `scripts/probe-stlink.sh`
- verify target voltage
- verify board is really `STM32F103C8T6`

## Step 3: Observe Expected Behavior

### 01_gpio_led

Pass:

- `PC13` LED behavior matches the example README

Fail:

- no LED response
- inverted or inconsistent LED behavior

First checks:

- board LED polarity
- `PC13` board wiring

### 02_oled_i2c

Pass:

- OLED powers up and shows the expected output

Fail:

- blank screen
- unstable refresh
- obvious addressing mismatch

First checks:

- OLED VCC/GND
- SDA/SCL pin wiring
- OLED controller variant

### 07_usart_cli

Serial settings:

- `115200 8N1`

Input to verify:

```text
help
led on
led off
led toggle
echo hello stm32
```

Pass:

- boot prompt appears
- commands parse correctly
- LED commands work
- echo output matches input

Fail:

- no prompt
- dropped characters
- command mismatch
- LED command ineffective

First checks:

- TX/RX crossover
- common ground
- terminal serial settings

## Step 4: Record Result

For each smoke example, record:

- flashed successfully or not
- observed phenomenon
- serial output if applicable
- mismatch against README
- probable cause if failed

## Completion Rule

Hardware closure is complete only when:

- ST-Link probe succeeds
- smoke examples flash
- observed behavior matches documentation
- remaining example risks are either cleared or explicitly documented
