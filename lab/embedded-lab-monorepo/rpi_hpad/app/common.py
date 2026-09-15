from __future__ import annotations

import argparse
import json
import time
from typing import Any

EVENT_SCHEMA = "rpi_hpad_event_v1"


def add_backend_args(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--backend",
        choices=["mock", "replay", "real"],
        default="mock",
        help="data source backend: mock generator, replay CSV, or reserved real hardware backend",
    )
    parser.add_argument("--config", help="JSON config path for the real backend")
    parser.add_argument("--csv", help="CSV file used by the replay backend")
    parser.add_argument(
        "--replay-loop",
        choices=["true", "false"],
        default="true",
        help="whether replay backend loops when reaching end-of-file",
    )


def parse_channels(value: str) -> list[int]:
    try:
        channels = [int(item) for item in value.split(",") if item]
    except ValueError as exc:
        raise argparse.ArgumentTypeError("channels must be a comma-separated list of integers") from exc
    if not channels:
        raise argparse.ArgumentTypeError("at least one channel is required")
    return channels


def replay_loop_enabled(value: str) -> bool:
    return value.lower() == "true"


def print_json(data: Any) -> None:
    print(json.dumps(data, ensure_ascii=True, sort_keys=True))


def event_record(event: str, **fields: Any) -> dict[str, Any]:
    record = {
        "schema": EVENT_SCHEMA,
        "ts": time.time(),
        "event": event,
    }
    record.update(fields)
    return record


def print_event(event: str, **fields: Any) -> None:
    print_json(event_record(event, **fields))
