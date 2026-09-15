# Protocol And Status Contract

Version: `rpi_hpad_event_v1`

Status: frozen for `v0.1.0-soft-validated`

Compatibility rule:

- new fields may be added
- stable top-level fields must not be renamed or removed within this protocol version
- stable event names and stable error codes must not be repurposed within this protocol version

## Stable Top-Level Fields

Every CLI JSON event must include:

- `schema`
- `ts`
- `event`
- `backend`

Meanings:

- `schema`: protocol identifier, currently `rpi_hpad_event_v1`
- `ts`: unix seconds as float
- `event`: event name
- `backend`: one of `mock`, `replay`, `real`

## Stable Event Names

- `status`
- `sample`
- `log_complete`
- `backend_unavailable`

## Stable Error Codes

- `real_backend_not_implemented`

## Status Event Contract

Top-level fields are integration-safe.

Backend-specific detail lives under `status`.

Example:

```json
{
  "schema": "rpi_hpad_event_v1",
  "ts": 1775897737.74852,
  "event": "status",
  "backend": "replay",
  "status": {
    "mode": "replay",
    "row_count": 5,
    "index": 0,
    "loop": true,
    "dac": {
      "0": 0.0,
      "1": 0.0
    }
  }
}
```

## Sample Event Contract

`channels` is a mapping from channel id to voltage.

Example:

```json
{
  "schema": "rpi_hpad_event_v1",
  "ts": 1775897737.811591,
  "event": "sample",
  "backend": "replay",
  "channels": {
    "0": 2.51,
    "1": 1.03
  }
}
```

## Backend Unavailable Contract

This event is the stable surface for unsupported runtime paths.

Example:

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
