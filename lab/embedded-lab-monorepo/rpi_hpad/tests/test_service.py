from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from drivers.mock_board import MockBoard
from services.analog_service import AnalogService


class AnalogServiceTest(unittest.TestCase):
    def test_read_average_in_range(self) -> None:
        service = AnalogService(MockBoard())
        value = service.read_average(0, 4)
        self.assertGreaterEqual(value, 0.0)
        self.assertLessEqual(value, 5.0)

    def test_mirror_adc_to_dac(self) -> None:
        board = MockBoard()
        service = AnalogService(board)
        mirrored = service.mirror_adc_to_dac(0, 1, 2)
        self.assertAlmostEqual(board.dac[1], mirrored)

    def test_log_to_csv(self) -> None:
        board = MockBoard()
        service = AnalogService(board)
        with tempfile.TemporaryDirectory() as tmpdir:
            output = Path(tmpdir) / "out.csv"
            service.log_to_csv(str(output), [0, 1], 2, 0.0)
            data = output.read_text(encoding="utf-8")
            self.assertIn("# rpi_hpad_csv_v1", data)
            self.assertIn("# metadata=", data)
            self.assertIn("timestamp,ch0,ch1", data)
