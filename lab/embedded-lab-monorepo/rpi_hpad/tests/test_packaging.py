from __future__ import annotations

import json
import subprocess
import tempfile
import unittest
from pathlib import Path

from drivers.config import RealBoardConfig


class PackagingTest(unittest.TestCase):
    def test_hpad_help_guides_first_steps(self) -> None:
        result = subprocess.run(
            ["python3", "-m", "app.main", "--help"],
            cwd="/home/qiaopai/rpi_hpad",
            check=True,
            capture_output=True,
            text=True,
        )
        self.assertIn("monitor", result.stdout)
        self.assertIn("log", result.stdout)
        self.assertIn("replay", result.stdout)
        self.assertIn("Suggested first steps", result.stdout)

    def test_protocol_doc_freezes_current_schema(self) -> None:
        protocol_doc = Path("/home/qiaopai/rpi_hpad/PROTOCOL.md").read_text(encoding="utf-8")
        self.assertIn("rpi_hpad_event_v1", protocol_doc)
        self.assertIn("real_backend_not_implemented", protocol_doc)

    def test_real_board_config_from_json(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            config_path = Path(tmpdir) / "real.json"
            config_path.write_text(json.dumps({"spi_bus": 10, "gain": 2}), encoding="utf-8")
            config = RealBoardConfig.from_json(str(config_path))
            self.assertEqual(config.spi_bus, 10)
            self.assertEqual(config.gain, 2)
            self.assertEqual(config.spi_device, 0)

    def test_hpad_status_mock(self) -> None:
        result = subprocess.run(
            ["python3", "-m", "app.main", "status", "--backend", "mock"],
            cwd="/home/qiaopai/rpi_hpad",
            check=True,
            capture_output=True,
            text=True,
        )
        payload = json.loads(result.stdout)
        self.assertEqual(payload["schema"], "rpi_hpad_event_v1")
        self.assertEqual(payload["event"], "status")
        self.assertEqual(payload["backend"], "mock")
        self.assertEqual(payload["status"]["mode"], "mock")
        self.assertIn("ts", payload)

    def test_hpad_status_real_is_honest(self) -> None:
        result = subprocess.run(
            ["python3", "-m", "app.main", "status", "--backend", "real"],
            cwd="/home/qiaopai/rpi_hpad",
            check=False,
            capture_output=True,
            text=True,
        )
        self.assertEqual(result.returncode, 2)
        payload = json.loads(result.stdout)
        self.assertEqual(payload["schema"], "rpi_hpad_event_v1")
        self.assertEqual(payload["event"], "backend_unavailable")
        self.assertEqual(payload["backend"], "real")
        self.assertEqual(payload["code"], "real_backend_not_implemented")

    def test_log_output_can_replay(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            output = Path(tmpdir) / "capture.csv"
            log_result = subprocess.run(
                [
                    "python3",
                    "-m",
                    "app.main",
                    "log",
                    "--backend",
                    "mock",
                    "--output",
                    str(output),
                    "--samples",
                    "2",
                    "--interval",
                    "0.0",
                ],
                cwd="/home/qiaopai/rpi_hpad",
                check=True,
                capture_output=True,
                text=True,
            )
            replay_result = subprocess.run(
                [
                    "python3",
                    "-m",
                    "app.main",
                    "replay",
                    "--csv",
                    str(output),
                    "--samples",
                    "2",
                    "--interval",
                    "0.0",
                    "--loop",
                    "false",
                ],
                cwd="/home/qiaopai/rpi_hpad",
                check=True,
                capture_output=True,
                text=True,
            )
            log_payload = json.loads(log_result.stdout)
            replay_lines = [json.loads(line) for line in replay_result.stdout.splitlines() if line.strip()]
            self.assertEqual(log_payload["event"], "log_complete")
            self.assertEqual(log_payload["schema"], "rpi_hpad_event_v1")
            self.assertEqual(replay_lines[0]["event"], "status")
            self.assertEqual(replay_lines[0]["backend"], "replay")
            self.assertEqual(replay_lines[1]["event"], "sample")
            self.assertEqual(replay_lines[1]["schema"], "rpi_hpad_event_v1")

    def test_release_notes_name_current_milestone(self) -> None:
        release_notes = Path("/home/qiaopai/rpi_hpad/RELEASE_NOTES.md").read_text(encoding="utf-8")
        changelog = Path("/home/qiaopai/rpi_hpad/CHANGELOG.md").read_text(encoding="utf-8")
        self.assertIn("v0.1.0-soft-validated", release_notes)
        self.assertIn("v0.1.0-soft-validated", changelog)
