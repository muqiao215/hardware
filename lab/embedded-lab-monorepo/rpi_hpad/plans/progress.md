# Progress

## 2026-04-11

### Session Start

Status: in_progress.

Completed:

- Chose a separate project root: `/home/qiaopai/rpi_hpad`
- Created initial directories for drivers, services, app, ros2, tests, data, scripts, and plans

Next:

- Implement the software-only architecture with mock/replay backends and verification

### Software-Only Backend Milestone

Status: complete / no_hardware_gate_green.

Completed:

- Added driver interface and backends:
  - `drivers/board_base.py`
  - `drivers/mock_board.py`
  - `drivers/replay_board.py`
  - `drivers/real_board.py`
- Added service layer:
  - `services/analog_service.py`
  - `services/filters.py`
  - `services/calibration.py`
- Added application entry points:
  - `app/cli_monitor.py`
  - `app/logger_app.py`
  - `app/factory.py`
- Added ROS2-facing entry:
  - `ros2_pkg/analog_io_node.py`
- Added replay sample data and tests:
  - `data/sample_replay.csv`
  - `tests/test_filters.py`
  - `tests/test_service.py`
  - `tests/test_mock_pipeline.py`
- Added no-hardware gate:
  - `scripts/check.sh`

Verified:

- `python3 -m unittest discover -s tests -v`
- `python3 -m app.cli_monitor --backend mock --samples 3 --interval 0.0`
- `./scripts/check.sh`

Behavior verified:

- mock backend produces bounded simulated voltages
- replay backend drives service logic from CSV
- logger writes CSV output
- ROS2 node falls back cleanly to console mode when `rclpy` is absent
- real backend raises `NotImplementedError` rather than pretending hardware exists

### Productized Package Milestone

Status: complete / no_hardware_gate_green.

Completed:

- Added installable package metadata:
  - `pyproject.toml`
  - `.gitignore`
- Added unified CLI/product entrypoints:
  - `app/main.py`
  - `app/common.py`
- Standardized backend selection across monitor/logger/status flows
- Added real-board config boundary without fake driver details:
  - `drivers/config.py`
  - `data/real_board.example.json`
- Updated backends and factory wiring so replay/logging flows work through the same board/service interface
- Added packaging coverage:
  - `tests/test_packaging.py`
- Hardened build gate for Debian-style externally managed Python environments:
  - `scripts/check.sh` now uses an isolated virtualenv for editable-install smoke tests

Verified:

- `python3 -m unittest discover -s tests -v`
- `./scripts/check.sh`

Behavior verified:

- `python3 -m app.main status --backend mock` now executes correctly via module entry
- editable install works through `pyproject.toml`
- `hpad` CLI works from an isolated installed environment
- replay/logger/status/ROS2 fallback smoke checks all pass through the packaging gate
- `real_board` still stops at explicit configuration and `NotImplementedError`, with no unverifiable SPI/GPIO claims

### CLI And Data UX Milestone

Status: complete / no_hardware_gate_green.

Completed:

- Unified CLI output and command surface:
  - `hpad monitor`
  - `hpad log`
  - `hpad replay`
  - `hpad status`
- Added JSON event-style output for status/sample/log completion paths
- Added replay command as a first-class product entry:
  - `app/replay_app.py`
  - `hpad-replay` console script
- Stabilized replay/log data contract:
  - CSV files now include `# rpi_hpad_csv_v1`
  - metadata comment line with JSON
  - replay loader skips metadata comments and reads the same file directly
- Improved status honesty:
  - `hpad status --backend real` now returns an explicit unavailable event and next-step guidance instead of a vague traceback
- Updated README so an unfamiliar user can discover the command model, CSV format, and current hardware boundary from docs alone
- Expanded test and gate coverage for:
  - log -> replay roundtrip
  - honest `real` backend status
  - metadata-bearing replay CSV input

Verified:

- `python3 -m unittest discover -s tests -v`
- `./scripts/check.sh`

Behavior verified:

- monitor, log, replay, and status now look like one coherent command family
- logger output can be replayed without manual conversion
- status output is machine-readable and explicit about backend state
- current project remains software-first; no unverifiable ADS1256/DAC8532 implementation was added

### Help And Event Protocol Milestone

Status: complete / no_hardware_gate_green.

Completed:

- Began versioning CLI JSON as `rpi_hpad_event_v1`
- Added stable top-level event fields:
  - `schema`
  - `ts`
  - `event`
  - `backend`
- Added explicit backend error code:
  - `real_backend_not_implemented`
- Started treating `hpad --help` as product entry rather than raw parser output
- Added README protocol section and golden-style packaging tests for help and JSON structure

Verified:

- `python3 -m unittest discover -s tests -v`
- `./scripts/check.sh`

Behavior verified:

- `hpad --help` now acts as a first-step guide instead of only listing parser options
- CLI JSON output now carries a stable top-level protocol:
  - `schema`
  - `ts`
  - `event`
  - `backend`
- backend-specific details are isolated under `status`, making the top-level JSON safer to depend on
- `backend_unavailable` now includes a stable error code for the reserved real backend path

### Release Freeze Milestone

Status: complete / v0.1.0-soft-validated / no_hardware_gate_green.

Completed:

- Froze current software-facing milestone as `v0.1.0-soft-validated`
- Added release surface:
  - `CHANGELOG.md`
  - `RELEASE_NOTES.md`
  - `QUICKSTART.md`
  - `PROTOCOL.md`
- Promoted `rpi_hpad_event_v1` from README section to explicit protocol/status contract
- Documented compatibility rule:
  - additive fields allowed
  - stable top-level fields must not be renamed or removed in this protocol version
  - stable event names and error codes must not be repurposed in this protocol version
- Added tests that lock the release docs and protocol identifiers

Verified:

- `python3 -m unittest discover -s tests -v`
- `./scripts/check.sh`

Behavior verified:

- package installs in isolated virtualenv
- `hpad --help` remains discoverable
- mock -> log -> replay flow works
- `rpi_hpad_event_v1` remains present in docs and JSON output
- `real` backend remains an honest pending boundary with `real_backend_not_implemented`

Hardware state:

- hardware_pending: no ADS1256/DAC8532/SPI/GPIO behavior is claimed or validated in this release
