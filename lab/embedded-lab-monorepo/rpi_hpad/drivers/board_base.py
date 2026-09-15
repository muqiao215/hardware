from __future__ import annotations

from abc import ABC, abstractmethod
from typing import Dict, Optional


class AnalogBoardBase(ABC):
    @abstractmethod
    def read_raw(self, channel: int) -> int:
        raise NotImplementedError

    @abstractmethod
    def read_voltage(self, channel: int) -> float:
        raise NotImplementedError

    @abstractmethod
    def write_voltage(self, channel: int, voltage: float) -> None:
        raise NotImplementedError

    @abstractmethod
    def get_status(self) -> Dict[str, object]:
        raise NotImplementedError

    def set_sampling_params(
        self, sample_rate_hz: Optional[float] = None, gain: Optional[float] = None
    ) -> None:
        del sample_rate_hz, gain

    def calibrate_zero(self, channel: int, offset_voltage: float) -> None:
        del channel, offset_voltage

    def step(self) -> None:
        """Advance frame-oriented backends such as replay data."""
