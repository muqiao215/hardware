from __future__ import annotations

import argparse

from app import cli_monitor, logger_app, replay_app
from app.common import add_backend_args, print_event
from app.factory import create_board


def main() -> int:
    parser = argparse.ArgumentParser(
        prog="hpad",
        description="RPi HPAD toolkit with mock, replay, and real hardware backends.",
        epilog=(
            "Suggested first steps: run 'hpad monitor --backend mock', then "
            "'hpad log --backend mock --output capture.csv', then "
            "'hpad replay --csv capture.csv'."
        ),
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    monitor_parser = subparsers.add_parser("monitor", help="stream channel snapshots")
    cli_monitor.configure_parser(monitor_parser)

    log_parser = subparsers.add_parser("log", help="record channel snapshots to CSV")
    logger_app.configure_parser(log_parser)

    replay_parser = subparsers.add_parser("replay", help="replay a CSV capture as sample output")
    replay_app.configure_parser(replay_parser)

    status_parser = subparsers.add_parser("status", help="show backend status only")
    add_backend_args(status_parser)

    args = parser.parse_args()

    if args.command == "monitor":
        return cli_monitor.run(args)
    if args.command == "log":
        return logger_app.run(args)
    if args.command == "replay":
        return replay_app.run(args)
    if args.command == "status":
        board = None
        try:
            board = create_board(
                args.backend,
                csv_path=args.csv,
                config_path=args.config,
                replay_loop=args.replay_loop == "true",
            )
        except Exception as exc:
            print_event(
                "backend_unavailable",
                backend=args.backend,
                code="backend_start_failed",
                reason=str(exc),
                next_step="check wiring, SPI/GPIO permissions, and real backend config",
            )
            return 2
        print_event("status", backend=args.backend, status=board.get_status())
        if hasattr(board, "close"):
            board.close()
        return 0
    raise ValueError(f"unsupported command: {args.command}")


if __name__ == "__main__":
    raise SystemExit(main())
