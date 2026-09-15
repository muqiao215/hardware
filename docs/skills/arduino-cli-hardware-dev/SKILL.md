---
name: arduino-cli-hardware-dev
description: This skill should be used when the user asks to use Arduino CLI (arduino-cli) to set up Arduino-compatible hardware development from the command line, including installing cores, configuring Boards Manager additional URLs, creating sketches, compiling, uploading/flashing, and monitoring serial output (Windows/macOS/Linux). Trigger phrases include: "arduino-cli", "Arduino CLI", "compile", "upload", "board list", "FQBN", "core install", "lib install", "additional-urls", "serial monitor", "ESP8266/3rd party core".
---

# Arduino CLI hardware-dev workflow

## Triage (ask first)

Collect only what is needed to run `arduino-cli` deterministically:

- Board model (or MCU family), and whether it is official Arduino vs 3rd-party (ESP8266, etc.).
- OS + connection type (USB serial / network / programmer).
- Port name (Windows: `COM3`, Linux/macOS: `/dev/tty*`) from `arduino-cli board list`.
- Target board FQBN (e.g. `arduino:avr:uno`). If unknown, determine via `board list` / `board listall`.
- Any required Boards Manager URL(s) for 3rd-party cores (vendor-provided “package_*.json”).

## Minimal end-to-end workflow (the “happy path”)

1. Verify CLI available:
   - `arduino-cli version`
2. Update core index (fresh install step):
   - `arduino-cli core update-index`
3. Detect connected board(s):
   - `arduino-cli board list --discovery-timeout 10s`
4. Install the core reported by `board list` (example):
   - `arduino-cli core install arduino:avr`
5. Create a new sketch:
   - `arduino-cli sketch new Blink`
6. Compile (set the FQBN for the target board):
   - `arduino-cli compile -b arduino:avr:uno Blink`
7. Upload (pick the correct port):
   - `arduino-cli upload -p COM3 -b arduino:avr:uno Blink`
   - Alternative (compile + upload in one command): `arduino-cli compile -b arduino:avr:uno -p COM3 -u Blink`
8. Monitor serial (discover settings first, then configure):
   - `arduino-cli monitor -p COM3 --describe`
   - `arduino-cli monitor -p COM3 -c baudrate=115200`

## “Unknown board” or missing FQBN

- If `arduino-cli board list` shows an `Unknown` board, uploads can still work when the correct FQBN is provided.
- Search for candidate boards and FQBNs:
  - `arduino-cli board listall uno`
  - `arduino-cli board listall mkr`

## Third-party cores via Boards Manager URLs

Prefer storing additional URLs in a project config so commands stay short.

- One-off (example from Arduino docs, ESP8266):
  - `arduino-cli core update-index --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json`
  - `arduino-cli core search esp8266 --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json`
- Offline/local index file:
  - `arduino-cli core update-index --additional-urls file:///absolute/path/to/package_index.json`

## Libraries

- Search:
  - `arduino-cli lib search debouncer`
- Install:
  - `arduino-cli lib install FTDebouncer`
- Install from Git/zip (treated as “unsafe install” in the configuration docs):
  - `arduino-cli lib install --git-url https://example.com/vendor/SomeLib.git`
  - `arduino-cli lib install --zip-path C:\\path\\to\\SomeLib.zip`

## Project-local config (recommended for reproducible builds)

Keep Arduino CLI state inside the repo to avoid cross-project pollution (cores, libraries, indexes).

- Create config:
  - `arduino-cli config init --dest-file .\\.arduino-cli\\arduino-cli.yaml --overwrite`
- Run with explicit config:
  - `arduino-cli --config-file .\\.arduino-cli\\arduino-cli.yaml core update-index`

Use `scripts/init-project.ps1` to scaffold a local config + example sketch without requiring `arduino-cli` to be installed.

## Bundled resources

- `skills/arduino-cli-hardware-dev/references/workflow.md`: Practical CLI flows (detect → install → compile → upload → monitor).
- `skills/arduino-cli-hardware-dev/references/config.md`: Minimal project-local `arduino-cli.yaml` and Windows defaults.
- `skills/arduino-cli-hardware-dev/references/wsl2.md`: WSL2 notes (USB serial access, ports, path differences).
- `skills/arduino-cli-hardware-dev/references/troubleshooting.md`: Common failure modes and fast checks.
- `skills/arduino-cli-hardware-dev/scripts/init-project.ps1`: Create `.arduino-cli/` + starter `sketches/` layout.
- `skills/arduino-cli-hardware-dev/examples/windows-uno-blink.ps1`: Example “compile + upload” for UNO on Windows.
- `skills/arduino-cli-hardware-dev/examples/esp8266-core-bootstrap.ps1`: Example “additional-urls + core search/install”.
