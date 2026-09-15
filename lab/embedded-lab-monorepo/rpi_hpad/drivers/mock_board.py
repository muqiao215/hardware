from __future__ import annotations

import math
import time
from typing import Dict

from drivers.board_base import AnalogBoardBase


class MockBoard(AnalogBoardBase):
    def __init__(self, vref: float = 5.0) -> None:
        self.start_time = time.monotonic()
        self.vref = vref
        self.dac = {0: 0.0, 1: 0.0}
        self.sample_rate_hz = 10.0
        self.gain = 1.0
        self.zero_offsets = {0: 0.0, 1: 0.0, 2: 0.0}

    def _signal(self, channel: int, t: float) -> float:
        if channel == 0:
            return 2.5 + math.sin(t)
        if channel == 1:
            return 1.0 + 0.2 * math.sin(t * 5.0)
        if channel == 2:
            return 0.5 + 0.1 * math.cos(t * 2.0)
        return 0.0

    def read_voltage(self, channel: int) -> float:
        t = time.monotonic() - self.start_time
        value = self._signal(channel, t) - self.zero_offsets.get(channel, 0.0)
        return max(0.0, min(self.vref, value))

    def read_raw(self, channel: int) -> int:
        return int((self.read_voltage(channel) / self.vref) * 0x7FFFFF)

    def write_voltage(self, channel: int, voltage: float) -> None:
        self.dac[channel] = max(0.0, min(self.vref, voltage))

    def get_status(self) -> Dict[str, object]:
        return {
            "mode": "mock",
            "sample_rate_hz": self.sample_rate_hz,
            "gain": self.gain,
            "dac": dict(self.dac),
        }

    def set_sampling_params(self, sample_rate_hz: float | None = None, gain: float | None = None) -> None:
        if sample_rate_hz is not None:
            self.sample_rate_hz = sample_rate_hz
        if gain is not None:
            self.gain = gain

    def calibrate_zero(self, channel: int, offset_voltage: float) -> None:
        self.zero_offsets[channel] = offset_voltage

    def step(self) -> None:
        return None
