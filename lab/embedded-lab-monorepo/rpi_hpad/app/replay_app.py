from __future__ import annotations

import argparse
import time

from app.common import parse_channels, print_event
from app.factory import create_board
from services.analog_service import AnalogService


def configure_parser(parser: argparse.ArgumentParser) -> None:
    parser.description = "Replay a CSV capture as a visible sample stream."
    parser.add_argument("--csv", required=True, help="CSV file to replay")
    parser.add_argument("--channels", type=parse_channels, default=parse_channels("0,1"), help="channels to display")
    parser.add_argument("--samples", type=int, default=5, help="number of replayed rows to emit")
    parser.add_argument("--interval", type=float, default=0.0, help="seconds between replayed rows")
    parser.add_argument(
        "--loop",
        choices=["true", "false"],
        default="true",
        help="whether replay loops at end-of-file",
    )


def run(args: argparse.Namespace) -> int:
    board = create_board("replay", csv_path=args.csv, replay_loop=args.loop == "true")
    service = AnalogService(board)

    print_event("status", backend="replay", csv=args.csv, status=board.get_status())
    for _ in range(args.samples):
        print_event("sample", backend="replay", channels=service.get_snapshot(args.channels))
        time.sleep(args.interval)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    configure_parser(parser)
    return run(parser.parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
