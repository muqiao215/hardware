# Release Notes: v0.1.0-soft-validated

This release freezes the software-facing surface for the Raspberry Pi High-Precision AD/DA Board toolkit.

What is ready:

- installable package
- unified `hpad` CLI
- mock backend for live software demos
- replay backend for CSV-driven reproduction
- logger that writes replay-compatible captures
- `rpi_hpad_event_v1` JSON output protocol
- no-hardware verification gate

What is intentionally not ready:

- real ADS1256 ADC driver
- real DAC8532 DAC driver
- SPI/GPIO timing validation
- true hardware calibration or accuracy claims

The `real` backend is reserved and explicit. It reports `backend_unavailable` with code `real_backend_not_implemented`.

Completion definition for this release:

- software package installs
- CLI commands run from an isolated virtual environment
- mock -> log -> replay workflow works
- JSON protocol has stable top-level fields
- real hardware path does not pretend to be implemented
