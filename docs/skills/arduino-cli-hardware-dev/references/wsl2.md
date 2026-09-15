# WSL2 (Ubuntu 22.04) notes for Arduino CLI

## Reality check: WSL2 cannot see Windows COM ports directly

Arduino CLI in WSL needs a Linux device node like:

- `/dev/ttyACM0` (CDC ACM)
- `/dev/ttyUSB0` (USB-serial: CH340/CP210x/FTDI)

If you only see `COM4` on Windows, WSL will not see it unless the USB device is passed through.

## Recommended options

### Option A (simplest): run `arduino-cli` on Windows

- Use Windows `arduino-cli` and `COMx`.
- Keep WSL for editing/build tools only.

### Option B: pass USB into WSL via `usbipd-win`

High-level workflow:

1. On Windows, list USB devices and attach the Arduino USB device to WSL.
2. In WSL, verify `/dev/ttyACM0` or `/dev/ttyUSB0` appears.
3. Run `arduino-cli upload -p /dev/ttyACM0 ...` from WSL.

Commands vary by device/driver. If `usbipd` is installed:

- Windows (PowerShell): `usbipd list`
- Windows (PowerShell): `usbipd bind --busid <BUSID>`
- Windows (PowerShell): `usbipd attach --wsl --busid <BUSID>`

Then in WSL:

- `ls -l /dev/ttyACM* /dev/ttyUSB*`

## Path differences

- Windows paths: `E:\\web\\...`
- WSL paths: `/mnt/e/web/...`

Example:

- `arduino-cli compile -b arduino:avr:nano /mnt/e/web/embedded/arduino_robot_arm`

