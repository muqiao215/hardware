# Troubleshooting checklist (fast)

## `arduino-cli` not found

- Ensure it is installed and in `PATH`.
- Quick check: `arduino-cli version`.

## Board not listed / wrong port

- Re-run with longer discovery timeout:
  - `arduino-cli board list --discovery-timeout 10s`
- Close Arduino IDE / serial monitor apps that may hold the port open.
- On Windows, verify the COM port exists in Device Manager.

## “Unknown” board in `board list`

- Install the platform core and use the correct FQBN manually:
  - `arduino-cli core install <PACKAGER:ARCH>`
  - `arduino-cli compile -b <FQBN> <sketch_dir>`
  - `arduino-cli upload -p <PORT> -b <FQBN> <sketch_dir>`

## Missing headers / libraries during compile

- Search and install:
  - `arduino-cli lib search <name>`
  - `arduino-cli lib install <LibraryName>`

## Weird behavior across projects

- Switch to project-local config so cores/libs are isolated:
  - `arduino-cli config init --dest-file .\\.arduino-cli\\arduino-cli.yaml --overwrite`
  - Use `--config-file` for every command.

