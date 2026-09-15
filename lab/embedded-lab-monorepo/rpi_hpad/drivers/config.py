from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict


@dataclass(frozen=True)
class RealBoardConfig:
    spi_bus: int = 0
    spi_device: int = 0
    vref: float = 5.0
    drdy_pin: int = 17
    reset_pin: int = 18
    pdwn_pin: int = 27
    cs_adc_pin: int = 22
    cs_dac_pin: int = 23
    sample_rate: str = "30SPS"
    gain: int = 1

    @classmethod
    def from_mapping(cls, values: Dict[str, Any]) -> "RealBoardConfig":
        known = {field: values[field] for field in cls.__dataclass_fields__ if field in values}
        return cls(**known)

    @classmethod
    def from_json(cls, path: str | None) -> "RealBoardConfig":
        if path is None:
            return cls()
        data = json.loads(Path(path).read_text(encoding="utf-8"))
        if not isinstance(data, dict):
            raise ValueError("real board config JSON must contain an object")
        return cls.from_mapping(data)
