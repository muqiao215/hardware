from __future__ import annotations

from typing import Iterable, List


def moving_average(values: Iterable[float]) -> float:
    samples = list(values)
    if not samples:
        raise ValueError("moving_average requires at least one sample")
    return sum(samples) / len(samples)


def median_filter(values: Iterable[float]) -> float:
    samples: List[float] = sorted(values)
    if not samples:
        raise ValueError("median_filter requires at least one sample")
    mid = len(samples) // 2
    if len(samples) % 2:
        return samples[mid]
    return (samples[mid - 1] + samples[mid]) / 2.0


def reject_outliers(values: Iterable[float], low: float, high: float) -> List[float]:
    return [value for value in values if low <= value <= high]
