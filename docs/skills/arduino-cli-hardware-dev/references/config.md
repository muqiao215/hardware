# Arduino CLI configuration notes

## Minimal project-local config example

Create `.\.arduino-cli\arduino-cli.yaml`, then run commands with `--config-file`.

Example:

```yaml
board_manager:
  additional_urls: []

directories:
  data: ./.arduino-cli/data
  downloads: ./.arduino-cli/staging
  user: ./.arduino-cli/user

output:
  no_color: true
```

If relative paths cause problems, replace them with absolute paths.

## Windows defaults (for orientation)

Arduino CLI uses OS-dependent defaults when not configured:

- `directories.data`: `{HOME}/AppData/Local/Arduino15`
- `directories.user`: `{DOCUMENTS}/Arduino`

These defaults matter when diagnosing “works on my machine” issues (wrong core/library picked up from global state).

