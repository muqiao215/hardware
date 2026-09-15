# Changelog

## v0.1.0-soft-validated

Released: 2026-04-11

Included:

- installable Python package via `pyproject.toml`
- unified `hpad` CLI with `monitor`, `log`, `replay`, and `status`
- `mock`, `replay`, and reserved `real` backend selection
- stable `rpi_hpad_event_v1` JSON event protocol
- replay-compatible CSV logging format with metadata header
- software-only verification gate through `scripts/check.sh`
- explicit `real` backend honesty path with stable unavailable code

Not included:

- verified ADS1256 SPI transactions
- verified DAC8532 output behavior
- verified GPIO, DRDY, or reset sequencing
- verified real-board sampling accuracy
