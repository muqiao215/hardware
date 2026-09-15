from __future__ import annotations

import argparse

from app.common import add_backend_args, parse_channels, print_event, replay_loop_enabled
from app.factory import create_board
from services.analog_service import AnalogService


def configure_parser(parser: argparse.ArgumentParser) -> None:
    parser.description = "Record channel snapshots to a replay-compatible CSV file."
    add_backend_args(parser)
    parser.add_argument("--output", required=True, help="output CSV path")
    parser.add_argument("--channels", type=parse_channels, default=parse_channels("0,1"), help="channels to record")
    parser.add_argument("--samples", type=int, default=10, help="number of rows to write")
    parser.add_argument("--interval", type=float, default=0.1, help="seconds between rows")


def run(args: argparse.Namespace) -> int:
    board = create_board(
        args.backend,
        csv_path=args.csv,
        config_path=args.config,
        replay_loop=replay_loop_enabled(args.replay_loop),
    )
    service = AnalogService(board)
    service.log_to_csv(args.output, args.channels, args.samples, args.interval)
    print_event(
        "log_complete",
        backend=args.backend,
        channels=args.channels,
        samples=args.samples,
        interval=args.interval,
        output=args.output,
    )
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    configure_parser(parser)
    return run(parser.parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
