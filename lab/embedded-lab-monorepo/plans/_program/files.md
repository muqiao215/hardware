# Migration File Map

## Control Files

- `plans/_program/task_plan.md`
- `plans/_program/progress.md`
- `plans/_program/findings.md`
- `plans/_program/files.md`

## Source Material

- `02软件安装及创建工程/`
- `03GPIO通用输入输出口/`
- `04OLED调试工具/`
- `05EXIT外部中断/`
- `06TIM定时器/`
- `07ADC模数转换器/`
- `08DMA直接存储器读取/`
- `09USART串口/`
- `10IIC通信/`
- `11SPI通信/`
- `pi_gcc_f103_blink/`

## Planned New Layout

- `examples/`
- `common/`
- `boards/`
- `cmake/`
- `scripts/build-all.sh`
- `scripts/flash.sh`
- `scripts/probe-stlink.sh`
- `scripts/package-validation-bundle.sh`
- `mcs51/`
- `mcs51/Learn_STC89C52/`
- `mcs51/README.md`
- `mcs51/UPSTREAM.md`
- `rpi_hpad/`
- `rpi_hpad/README.md`
- `rpi_hpad/QUICKSTART.md`
- `rpi_hpad/PROTOCOL.md`
- `rpi_hpad/RELEASE_NOTES.md`
- `rpi_hpad/CHANGELOG.md`
- `scripts/check-all.sh`
- `VERSION`
- `CHANGELOG.md`
- `RELEASE_NOTES.md`
- `HARDWARE_PENDING.md`
- `COMPATIBILITY.md`
- `REALBOARD_VALIDATION_CHECKLIST.md`

## Implemented Example Layout

- `examples/01_gpio_led/`
- `examples/02_oled_i2c/`
- `examples/03_exti_key/`
- `examples/04_tim_pwm/`
- `examples/05_adc_polling/`
- `examples/06_adc_dma/`
- `examples/07_usart_cli/`
- `examples/08_i2c_hw/`
- `examples/09_spi_loopback/`

## Shared Runtime Layout

- `common/cmsis/`
- `boards/stm32f103c8t6/`
- `common/core/`
- `common/drivers/`
- `common/components/`
- `common/cli/`
- `common/utils/`
- `common/stdperiph/`
- `common/bsp/` (legacy shared bucket kept as inactive compatibility/source material)
- `cmake/stm32f103_example.cmake`
- `cmake/toolchains/arm-gcc.cmake`
