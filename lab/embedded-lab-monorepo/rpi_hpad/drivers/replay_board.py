from __future__ import annotations

import csv
from pathlib import Path
from typing import Dict, List

from drivers.board_base import AnalogBoardBase


class ReplayBoard(AnalogBoardBase):
    def __init__(self, csv_path: str, vref: float = 5.0, loop: bool = True) -> None:
        self.vref = vref
        self.loop = loop
        self.rows = self._load_rows(csv_path)
        if not self.rows:
            raise ValueError("replay csv contained no rows")
        self.index = 0
        self.dac = {0: 0.0, 1: 0.0}

    def _load_rows(self, csv_path: str) -> List[Dict[str, float]]:
        path = Path(csv_path)
        rows: List[Dict[str, float]] = []
        with path.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.DictReader(line for line in handle if not line.startswith("#"))
            for row in reader:
                rows.append({key: float(value) for key, value in row.items()})
        return rows

    def _current_row(self) -> Dict[str, float]:
        return self.rows[self.index]

    def step(self) -> None:
        if self.index + 1 < len(self.rows):
            self.index += 1
        elif self.loop:
            self.index = 0

    def read_voltage(self, channel: int) -> float:
        row = self._current_row()
        return row.get(f"ch{channel}", 0.0)

    def read_raw(self, channel: int) -> int:
        return int((self.read_voltage(channel) / self.vref) * 0x7FFFFF)

    def write_voltage(self, channel: int, voltage: float) -> None:
        self.dac[channel] = max(0.0, min(self.vref, voltage))

    def get_status(self) -> Dict[str, object]:
        return {
            "mode": "replay",
            "row_count": len(self.rows),
            "index": self.index,
            "loop": self.loop,
            "dac": dict(self.dac),
        }
