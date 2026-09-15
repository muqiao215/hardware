from __future__ import annotations

import csv
import json
import time
from pathlib import Path
from typing import Dict, Iterable, List

from drivers.board_base import AnalogBoardBase
from services.calibration import apply_offset, clamp_voltage
from services.filters import median_filter, moving_average, reject_outliers


class AnalogService:
    def __init__(self, board: AnalogBoardBase, channel_offsets: Dict[int, float] | None = None) -> None:
        self.board = board
        self.channel_offsets = channel_offsets or {}

    def read_samples(self, channel: int, samples: int = 8) -> List[float]:
        values = []
        for _ in range(samples):
            values.append(self.board.read_voltage(channel))
            self.board.step()
        return values

    def read_average(self, channel: int, samples: int = 8) -> float:
        return moving_average(self.read_samples(channel, samples))

    def read_median(self, channel: int, samples: int = 5) -> float:
        return median_filter(self.read_samples(channel, samples))

    def read_filtered_average(
        self, channel: int, samples: int = 8, low: float = 0.0, high: float = 5.0
    ) -> float:
        kept = reject_outliers(self.read_samples(channel, samples), low, high)
        if not kept:
            raise ValueError("all samples were rejected as outliers")
        return moving_average(kept)

    def read_calibrated_voltage(self, channel: int, samples: int = 8) -> float:
        value = self.read_average(channel, samples)
        return apply_offset(value, self.channel_offsets.get(channel, 0.0))

    def mirror_adc_to_dac(self, adc_channel: int, dac_channel: int, samples: int = 4) -> float:
        voltage = clamp_voltage(self.read_average(adc_channel, samples))
        self.board.write_voltage(dac_channel, voltage)
        return voltage

    def get_snapshot(self, channels: Iterable[int]) -> Dict[int, float]:
        snapshot = {
            channel: apply_offset(
                self.board.read_voltage(channel), self.channel_offsets.get(channel, 0.0)
            )
            for channel in channels
        }
        self.board.step()
        return snapshot

    def log_to_csv(self, output_path: str, channels: Iterable[int], samples: int, interval_s: float) -> None:
        path = Path(output_path)
        channel_list = list(channels)
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8", newline="") as handle:
            metadata = {
                "format": "rpi_hpad_csv_v1",
                "timestamp": "unix_seconds",
                "channels": channel_list,
                "source_status": self.board.get_status(),
            }
            handle.write("# rpi_hpad_csv_v1\n")
            handle.write(f"# metadata={json.dumps(metadata, ensure_ascii=True, sort_keys=True)}\n")
            writer = csv.writer(handle)
            writer.writerow(["timestamp", *[f"ch{ch}" for ch in channel_list]])
            for _ in range(samples):
                row = [time.time()]
                snapshot = self.get_snapshot(channel_list)
                row.extend(snapshot[ch] for ch in channel_list)
                writer.writerow(row)
                time.sleep(interval_s)
