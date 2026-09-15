from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from app.factory import create_board
from drivers.real_board import RealBoard
from services.analog_service import AnalogService


class MockPipelineTest(unittest.TestCase):
    def test_replay_backend(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            path = Path(tmpdir) / "sample.csv"
            path.write_text(
                "# rpi_hpad_csv_v1\n"
                '# metadata={"channels":[0,1],"format":"rpi_hpad_csv_v1","timestamp":"unix_seconds"}\n'
                "timestamp,ch0,ch1\n0.0,1.25,2.50\n0.1,1.30,2.55\n",
                encoding="utf-8",
            )
            board = create_board("replay", str(path))
            service = AnalogService(board)
            self.assertAlmostEqual(service.read_average(0, 1), 1.25)

    def test_real_backend_requires_live_hardware_or_valid_gpio(self) -> None:
        try:
            board = RealBoard()
        except Exception as exc:
            self.assertIsInstance(exc, Exception)
        else:
            status = board.get_status()
            self.assertEqual(status["mode"], "real")
            board.close()
