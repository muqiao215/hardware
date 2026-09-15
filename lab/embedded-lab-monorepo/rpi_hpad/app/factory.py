from __future__ import annotations

from drivers.board_base import AnalogBoardBase
from drivers.config import RealBoardConfig
from drivers.mock_board import MockBoard
from drivers.real_board import RealBoard
from drivers.replay_board import ReplayBoard


def create_board(
    backend: str,
    csv_path: str | None = None,
    config_path: str | None = None,
    replay_loop: bool = True,
) -> AnalogBoardBase:
    if backend == "mock":
        return MockBoard()
    if backend == "replay":
        if not csv_path:
            raise ValueError("replay backend requires --csv")
        return ReplayBoard(csv_path, loop=replay_loop)
    if backend == "real":
        return RealBoard(RealBoardConfig.from_json(config_path))
    raise ValueError(f"unknown backend: {backend}")
