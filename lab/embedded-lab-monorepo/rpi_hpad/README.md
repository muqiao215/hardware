# rpi_hpad

Software-only scaffold for a Raspberry Pi High-Precision AD/DA Board workflow.

Release status:

- current milestone: `v0.1.0-soft-validated`
- protocol status: `rpi_hpad_event_v1` frozen for this release
- hardware status: `real` backend reserved only, still hardware-pending

Current state:

- stable board interface
- `MockBoard` backend
- `ReplayBoard` backend
- `RealBoard` placeholder only
- service layer for sampling, filtering, calibration, logging
- CLI monitor and CSV logger apps
- ROS2 node entry with graceful fallback if `rclpy` is missing
- no-hardware tests
- installable package metadata
- unified `hpad` CLI entry
- replay-compatible CSV logging format
- explicit `real` backend boundary with honest status reporting

Not yet claimed:

- SPI timing verified
- ADC conversion accuracy verified
- DAC output verified
- GPIO/DRDY behavior verified

## Quick Start

See also:

- [QUICKSTART.md](QUICKSTART.md) for the 5-minute path
- [PROTOCOL.md](PROTOCOL.md) for JSON/status compatibility
- [RELEASE_NOTES.md](RELEASE_NOTES.md) for release scope
- [CHANGELOG.md](CHANGELOG.md) for milestone history

Run tests:

```bash
python3 -m unittest discover -s tests -v
```

Install in editable mode:

```bash
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -e .
```

Inspect the command surface:

```bash
hpad --help
hpad monitor --help
hpad log --help
hpad replay --help
```

Monitor mock data:

```bash
hpad monitor --backend mock --samples 5
```

Log mock data to a replay-compatible CSV:

```bash
hpad log --backend mock --output data/out.csv --samples 5
```

Replay a CSV capture:

```bash
hpad replay --csv data/out.csv --samples 5 --interval 0.0 --loop false
```

Show backend status:

```bash
hpad status --backend mock
hpad status --backend replay --csv data/sample_replay.csv
hpad status --backend real
```

## Command Model

All subcommands use the same backend vocabulary:

- `--backend mock` for generated test signals
- `--backend replay --csv ...` for CSV playback
- `--backend real --config ...` for the reserved hardware backend

Current subcommands:

- `hpad monitor`: print JSON status and sample events
- `hpad log`: write replay-compatible CSV captures
- `hpad replay`: play back a capture as JSON sample events
- `hpad status`: print backend status or an honest unavailable message

## CSV Format

`hpad log` writes a stable replay format:

- line 1: `# rpi_hpad_csv_v1`
- line 2: `# metadata=...` JSON
- header row: `timestamp,ch0,ch1,...`
- data rows: unix seconds plus channel voltages

`ReplayBoard` ignores metadata comment lines and reads the same CSV directly.

## Status Semantics

The CLI always reports what is happening:

- which backend is active
- replay position for CSV playback
- output path for logging
- why `real` is unavailable today

`real` currently returns an explicit unavailable message instead of pretending hardware support exists.

## JSON Event Protocol

CLI output is a small machine-readable protocol.

Stable fields in every event:

- `schema`: currently `rpi_hpad_event_v1`
- `ts`: unix seconds as a floating-point timestamp
- `event`: event name

Stable event names:

- `status`: backend status snapshot
- `sample`: one ADC sample snapshot
- `log_complete`: logger finished writing a capture
- `backend_unavailable`: requested backend cannot run in this environment

Stable backend values:

- `mock`
- `replay`
- `real`

Status event shape:

```json
{
  "schema": "rpi_hpad_event_v1",
  "ts": 0.0,
  "event": "status",
  "backend": "mock",
  "status": {
    "mode": "mock"
  }
}
```

Error event shape:

```json
{
  "schema": "rpi_hpad_event_v1",
  "ts": 0.0,
  "event": "backend_unavailable",
  "backend": "real",
  "code": "real_backend_not_implemented",
  "reason": "...",
  "next_step": "use --backend mock or --backend replay until hardware integration exists"
}
```

Backend-specific details live under `status`. Top-level fields are the stable integration surface.
