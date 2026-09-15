# Hardware Pending Checklist

Current status: `hardware_pending`

Reference execution document:

- `REALBOARD_VALIDATION_CHECKLIST.md`

## Probe Status

- Latest observed result:

```text
Found 0 stlink programmers
```

## Board-Side Verification Still Required

- `01_gpio_led`
  - Confirm `PC13` LED behavior matches README
- `02_oled_i2c`
  - Confirm OLED wiring
  - Confirm screen refresh and drawing primitives on real display
- `03_exti_key`
  - Confirm `PA0` key wiring
  - Confirm debounce threshold on the actual button
- `04_tim_pwm`
  - Confirm PWM output pin and observed waveform or brightness behavior
- `05_adc_polling`
  - Confirm ADC pin input range and observed converted values
- `06_adc_dma`
  - Confirm DMA-backed ADC updates behave as expected on the board
- `07_usart_cli`
  - Confirm serial wiring
  - Confirm prompt, command parsing, and LED control over UART
- `08_i2c_hw`
  - Confirm hardware I2C wiring and bus transactions
- `09_spi_loopback`
  - Confirm SPI1 wiring and loopback behavior

## Environment-Side Checks Still Required

- ST-Link visibility from the Raspberry Pi
- `st-flash` write path on the current host
- Real board pin mapping confirmation against the specific Blue Pill board in use

## Completion Condition

This file can be closed only after:

- `scripts/probe-stlink.sh` detects at least one programmer
- at least the smoke examples are flashed and observed:
  - `01_gpio_led`
  - `02_oled_i2c`
  - `07_usart_cli`
- README-described phenomena match real hardware behavior
