from __future__ import annotations

import time
from typing import Dict

import lgpio
import spidev

from drivers.board_base import AnalogBoardBase
from drivers.config import RealBoardConfig

ADS1256_STATUS = 0x00
ADS1256_MUX = 0x01
ADS1256_ADCON = 0x02
ADS1256_DRATE = 0x03

ADS1256_CMD_RDATA = 0x01
ADS1256_CMD_RREG = 0x10
ADS1256_CMD_WREG = 0x50
ADS1256_CMD_SYNC = 0xFC
ADS1256_CMD_WAKEUP = 0x00

ADS1256_CHIP_ID = 0x03
ADS1256_POS_AINCOM = 0x08
ADS1256_FULL_SCALE = 0x7FFFFF

ADS1256_DRATE_CODES = {
    "30000SPS": 0xF0,
    "15000SPS": 0xE0,
    "7500SPS": 0xD0,
    "3750SPS": 0xC0,
    "2000SPS": 0xB0,
    "1000SPS": 0xA1,
    "500SPS": 0x92,
    "100SPS": 0x82,
    "60SPS": 0x72,
    "50SPS": 0x63,
    "30SPS": 0x53,
    "25SPS": 0x43,
    "15SPS": 0x33,
    "10SPS": 0x20,
    "5SPS": 0x13,
    "2.5SPS": 0x03,
    "2d5SPS": 0x03,
}

ADS1256_GAIN_CODES = {
    1: 0,
    2: 1,
    4: 2,
    8: 3,
    16: 4,
    32: 5,
    64: 6,
}

DAC8532_CHANNEL_A = 0x30
DAC8532_CHANNEL_B = 0x34
DAC8532_FULL_SCALE = 0xFFFF


class RealBoard(AnalogBoardBase):
    def __init__(self, config: RealBoardConfig | None = None) -> None:
        self.config = config or RealBoardConfig()
        self._closed = False
        self._gpio = lgpio.gpiochip_open(0)
        self._spi = spidev.SpiDev()
        self._spi.open(self.config.spi_bus, self.config.spi_device)
        self._spi.max_speed_hz = 20_000
        self._spi.mode = 0b01
        self._sample_rate_hz = self._sample_rate_to_hz(self.config.sample_rate)
        self._gain = float(self.config.gain)
        self._zero_offsets: Dict[int, float] = {}
        self._dac = {0: 0.0, 1: 0.0}
        self._initialized = False
        try:
            self._claim_gpio()
            self._hardware_reset()
            chip_id = self._read_chip_id()
            if chip_id != ADS1256_CHIP_ID:
                raise RuntimeError(f"unexpected ADS1256 chip id {chip_id}, expected {ADS1256_CHIP_ID}")
            self._configure_adc()
            self._initialized = True
        except Exception:
            self.close()
            raise

    def _claim_gpio(self) -> None:
        lgpio.gpio_claim_output(self._gpio, self.config.reset_pin)
        lgpio.gpio_claim_output(self._gpio, self.config.cs_adc_pin)
        lgpio.gpio_claim_output(self._gpio, self.config.cs_dac_pin)
        lgpio.gpio_claim_output(self._gpio, self.config.pdwn_pin)
        lgpio.gpio_claim_input(self._gpio, self.config.drdy_pin)
        lgpio.gpio_write(self._gpio, self.config.cs_adc_pin, 1)
        lgpio.gpio_write(self._gpio, self.config.cs_dac_pin, 1)
        lgpio.gpio_write(self._gpio, self.config.pdwn_pin, 1)

    def _hardware_reset(self) -> None:
        lgpio.gpio_write(self._gpio, self.config.reset_pin, 1)
        time.sleep(0.2)
        lgpio.gpio_write(self._gpio, self.config.reset_pin, 0)
        time.sleep(0.2)
        lgpio.gpio_write(self._gpio, self.config.reset_pin, 1)
        time.sleep(0.2)

    def _wait_drdy(self, timeout_s: float = 1.0) -> None:
        deadline = time.monotonic() + timeout_s
        while time.monotonic() < deadline:
            if lgpio.gpio_read(self._gpio, self.config.drdy_pin) == 0:
                return
            time.sleep(0.00001)
        raise TimeoutError("ADS1256 DRDY timeout")

    def _select_adc(self) -> None:
        lgpio.gpio_write(self._gpio, self.config.cs_adc_pin, 0)

    def _deselect_adc(self) -> None:
        lgpio.gpio_write(self._gpio, self.config.cs_adc_pin, 1)

    def _select_dac(self) -> None:
        lgpio.gpio_write(self._gpio, self.config.cs_dac_pin, 0)

    def _deselect_dac(self) -> None:
        lgpio.gpio_write(self._gpio, self.config.cs_dac_pin, 1)

    def _write_adc_cmd(self, cmd: int) -> None:
        self._select_adc()
        try:
            self._spi.writebytes([cmd])
        finally:
            self._deselect_adc()

    def _write_adc_reg(self, reg: int, value: int) -> None:
        self._select_adc()
        try:
            self._spi.writebytes([ADS1256_CMD_WREG | reg, 0x00, value & 0xFF])
        finally:
            self._deselect_adc()

    def _read_adc_reg(self, reg: int) -> int:
        self._select_adc()
        try:
            self._spi.writebytes([ADS1256_CMD_RREG | reg, 0x00])
            return self._spi.readbytes(1)[0]
        finally:
            self._deselect_adc()

    def _read_chip_id(self) -> int:
        self._wait_drdy()
        return (self._read_adc_reg(ADS1256_STATUS) >> 4) & 0x0F

    def _configure_adc(self) -> None:
        gain_code = ADS1256_GAIN_CODES.get(self.config.gain)
        if gain_code is None:
            raise ValueError(f"unsupported gain: {self.config.gain}")
        drate_code = ADS1256_DRATE_CODES.get(self.config.sample_rate.upper())
        if drate_code is None:
            raise ValueError(f"unsupported sample_rate: {self.config.sample_rate}")
        buf = [
            (0 << 3) | (1 << 2) | (0 << 1),
            ADS1256_POS_AINCOM,
            gain_code,
            drate_code,
        ]
        self._wait_drdy()
        self._select_adc()
        try:
            self._spi.writebytes([ADS1256_CMD_WREG | ADS1256_STATUS, 0x03])
            self._spi.writebytes(buf)
        finally:
            self._deselect_adc()
        time.sleep(0.001)

    def _set_channel(self, channel: int) -> None:
        if channel < 0 or channel > 7:
            raise ValueError("ADS1256 single-ended channel must be in range 0..7")
        self._write_adc_reg(ADS1256_MUX, (channel << 4) | ADS1256_POS_AINCOM)

    def _read_adc_data(self) -> int:
        self._wait_drdy()
        self._select_adc()
        try:
            self._spi.writebytes([ADS1256_CMD_RDATA])
            raw = self._spi.readbytes(3)
        finally:
            self._deselect_adc()
        value = ((raw[0] << 16) | (raw[1] << 8) | raw[2]) & 0xFFFFFF
        if value & 0x800000:
            value -= 1 << 24
        return value

    def _read_channel_raw(self, channel: int) -> int:
        self._set_channel(channel)
        self._write_adc_cmd(ADS1256_CMD_SYNC)
        self._write_adc_cmd(ADS1256_CMD_WAKEUP)
        return self._read_adc_data()

    def _raw_to_voltage(self, raw: int) -> float:
        return (raw / ADS1256_FULL_SCALE) * self.config.vref

    def _voltage_to_dac_code(self, voltage: float) -> int:
        clamped = max(0.0, min(self.config.vref, voltage))
        return int((clamped / self.config.vref) * DAC8532_FULL_SCALE)

    def _write_dac_raw(self, channel: int, code: int) -> None:
        if channel == 0:
            command = DAC8532_CHANNEL_A
        elif channel == 1:
            command = DAC8532_CHANNEL_B
        else:
            raise ValueError("DAC8532 channel must be 0 or 1")
        self._select_dac()
        try:
            self._spi.writebytes([command, (code >> 8) & 0xFF, code & 0xFF])
        finally:
            self._deselect_dac()

    def read_raw(self, channel: int) -> int:
        return self._read_channel_raw(channel)

    def read_voltage(self, channel: int) -> float:
        value = self._raw_to_voltage(self.read_raw(channel))
        return value - self._zero_offsets.get(channel, 0.0)

    def write_voltage(self, channel: int, voltage: float) -> None:
        clamped = max(0.0, min(self.config.vref, voltage))
        self._write_dac_raw(channel, self._voltage_to_dac_code(clamped))
        self._dac[channel] = clamped

    def get_status(self) -> dict:
        return {
            "mode": "real",
            "initialized": self._initialized,
            "spi_bus": self.config.spi_bus,
            "spi_device": self.config.spi_device,
            "sample_rate": self.config.sample_rate,
            "sample_rate_hz": self._sample_rate_hz,
            "gain": self._gain,
            "vref": self.config.vref,
            "pins": {
                "drdy": self.config.drdy_pin,
                "reset": self.config.reset_pin,
                "pdwn": self.config.pdwn_pin,
                "cs_adc": self.config.cs_adc_pin,
                "cs_dac": self.config.cs_dac_pin,
            },
            "adc_chip_id": self._read_chip_id(),
            "drdy_state": lgpio.gpio_read(self._gpio, self.config.drdy_pin),
            "dac": dict(self._dac),
        }

    def set_sampling_params(
        self, sample_rate_hz: float | None = None, gain: float | None = None
    ) -> None:
        if sample_rate_hz is not None:
            self._sample_rate_hz = sample_rate_hz
        if gain is not None:
            self._gain = gain

    def calibrate_zero(self, channel: int, offset_voltage: float) -> None:
        self._zero_offsets[channel] = offset_voltage

    def step(self) -> None:
        return None

    def close(self) -> None:
        if self._closed:
            return
        self._closed = True
        for pin in (
            self.config.drdy_pin,
            self.config.reset_pin,
            self.config.pdwn_pin,
            self.config.cs_adc_pin,
            self.config.cs_dac_pin,
        ):
            try:
                lgpio.gpio_free(self._gpio, pin)
            except Exception:
                pass
        try:
            self._spi.close()
        except Exception:
            pass
        try:
            lgpio.gpiochip_close(self._gpio)
        except Exception:
            pass

    def __del__(self) -> None:
        self.close()

    @staticmethod
    def _sample_rate_to_hz(sample_rate: str) -> float:
        text = sample_rate.upper().replace("SPS", "")
        text = text.replace("D", ".")
        try:
            return float(text)
        except ValueError:
            return 0.0
