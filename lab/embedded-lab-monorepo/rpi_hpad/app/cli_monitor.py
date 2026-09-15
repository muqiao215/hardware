from __future__ import annotations

import argparse
import time

from app.common import add_backend_args, parse_channels, print_event, replay_loop_enabled
from app.factory import create_board
from services.analog_service import AnalogService


def configure_parser(parser: argparse.ArgumentParser) -> None:
    parser.description = "Stream channel snapshots from a selected backend."
    add_backend_args(parser)
    parser.add_argument("--channels", type=parse_channels, default=parse_channels("0,1"), help="channels to read")
    parser.add_argument("--samples", type=int, default=10, help="number of snapshots to print")
    parser.add_argument("--interval", type=float, default=0.2, help="seconds between snapshots")


def run(args: argparse.Namespace) -> int:
    board = create_board(
        args.backend,
        csv_path=args.csv,
        config_path=args.config,
        replay_loop=replay_loop_enabled(args.replay_loop),
    )
    service = AnalogService(board)

    try:
        print_event("status", backend=args.backend, status=board.get_status())
        for _ in range(args.samples):
            snapshot = service.get_snapshot(args.channels)
            print_event("sample", backend=args.backend, channels=snapshot)
            time.sleep(args.interval)
    finally:
        if hasattr(board, "close"):
            board.close()
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    configure_parser(parser)
    return run(parser.parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
