# 01_blink

Goal:

- Prove the first MCS-51 command-line build path with SDCC.
- Generate `firmware.ihx` and `firmware.hex`.

Assumptions:

- MCU: `STC89C52`
- Clock: `11.0592 MHz` typical course board
- LED: `P2.0`

Build:

```bash
mcs51/scripts/build.sh 01_blink
```

Expected artifacts:

- `mcs51/build/01_blink/firmware.ihx`
- `mcs51/build/01_blink/firmware.hex`

Flash:

```bash
mcs51/scripts/flash.sh 01_blink /dev/ttyUSB0
```

Status:

- build is `toolchain_pending` until `sdcc` and `packihx` are installed
- flash/runtime is `hardware_pending` until a real STC board is connected
