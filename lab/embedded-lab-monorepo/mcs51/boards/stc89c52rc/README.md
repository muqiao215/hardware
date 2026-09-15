# STC89C52RC Board Profile

This is the first minimal board profile for the MCS-51 migration line.

Current assumptions:

- MCU family: 8051-compatible STC89C52
- Clock: course examples commonly assume `11.0592 MHz`
- First LED pin: `P2.0`
- UART0 TX pin: `P3.1`
- Current default key pin for `04_key_debounce`: `P3.1` via board alias in `board.h`
- Flash path: STC UART bootloader through `stcgal`

Status:

- build path: software-verified
- flash path: `hardware_pending` until a real board and serial adapter are verified

Important:

- `P3.1` as key input conflicts with classic 8051 `TXD`
- keep all key-pin assumptions behind `board.h` so real-board remapping is a one-line change
