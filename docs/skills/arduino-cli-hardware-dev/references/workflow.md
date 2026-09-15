# Arduino CLI workflow patterns

## Standard “compile + upload” loop

1. Detect port and guess FQBN:
   - `arduino-cli board list --discovery-timeout 10s`
2. If core is missing, install it:
   - `arduino-cli core install <PACKAGER:ARCH>`
3. Compile:
   - `arduino-cli compile -b <FQBN> <sketch_dir>`
4. Upload:
   - `arduino-cli upload -p <PORT> -b <FQBN> <sketch_dir>`
5. Serial monitor:
   - `arduino-cli monitor -p <PORT> --describe`
   - `arduino-cli monitor -p <PORT> -c baudrate=115200`

## Find the correct FQBN when the board is not detected

- List boards you can currently target (platforms already installed):
  - `arduino-cli board listall`
- Search by keyword:
  - `arduino-cli board listall nano`
  - `arduino-cli board listall esp`

## Use additional Boards Manager URLs

Use `--additional-urls` for one-off operations, or store it in config for repeated usage.

- One-off:
  - `arduino-cli core update-index --additional-urls <URL_TO_PACKAGE_INDEX_JSON>`
  - `arduino-cli core search <term> --additional-urls <URL_TO_PACKAGE_INDEX_JSON>`
- Config-first (preferred):
  - Put the URL under `board_manager.additional_urls` in the config.
  - Always run with `--config-file`.

## “Reproducible build” defaults

Aim for predictable state:

- Put CLI state under repo: `.arduino-cli/data` and `.arduino-cli/user`.
- Run every command with `--config-file .\\.arduino-cli\\arduino-cli.yaml`.
- Export binaries for CI artifacts:
  - `arduino-cli compile -b <FQBN> --export-binaries <sketch_dir>`

