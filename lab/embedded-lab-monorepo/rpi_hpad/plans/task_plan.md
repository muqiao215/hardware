# RPi HPAD Software-Only Plan

## Goal

Build the no-hardware software half of a Raspberry Pi High-Precision AD/DA Board stack so that real hardware can be added later by replacing only the lowest backend layer.

## Scope

- Define a stable board interface.
- Implement `mock` and `replay` backends.
- Add a `real` backend placeholder that explicitly refuses use without hardware integration.
- Build service-layer sampling, filtering, calibration, logging, and CLI monitoring.
- Add a ROS2-facing node entry with graceful fallback when ROS2 is unavailable.
- Add automated tests for the no-hardware pipeline.

## Non-Goals

- Do not claim SPI/GPIO/DRDY timing is verified.
- Do not claim ADC/DAC values are hardware-accurate.
- Do not bundle a real ADS1256/DAC8532 implementation yet.

## Phases

1. Create project skeleton and planning files.
2. Implement driver interfaces and backends.
3. Implement services and applications.
4. Implement ROS2 stub/integration entry.
5. Add tests and run verification.
