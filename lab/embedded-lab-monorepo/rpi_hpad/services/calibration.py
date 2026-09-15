from __future__ import annotations


def apply_offset(voltage: float, offset: float) -> float:
    return voltage - offset


def clamp_voltage(voltage: float, minimum: float = 0.0, maximum: float = 5.0) -> float:
    return max(minimum, min(maximum, voltage))
