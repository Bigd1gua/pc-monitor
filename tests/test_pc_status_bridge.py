#!/usr/bin/env python3
"""Host tests for the PC status bridge's pure logic and safety guards."""

from __future__ import annotations

import importlib.util
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]

# The bridge imports pyserial at module load, so skip cleanly where it is absent
# (for example the CI static-checks job, which does not install pyserial).
if importlib.util.find_spec("serial") is None:
    print("SKIP: pyserial is not installed; bridge tests skipped")
    raise SystemExit(0)

SPEC = importlib.util.spec_from_file_location(
    "pc_status_bridge", ROOT / "tools" / "pc_status_bridge.py"
)
assert SPEC and SPEC.loader
BRIDGE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(BRIDGE)


class _FakePort:
    def __init__(self, device: str, vid: int | None = None, description: str = "") -> None:
        self.device = device
        self.vid = vid
        self.description = description


class SanitizeTest(unittest.TestCase):
    def test_replaces_frame_separators(self):
        self.assertEqual(BRIDGE.sanitize("a,b#c=d"), "a_b_c_d")

    def test_strips_and_truncates(self):
        self.assertEqual(BRIDGE.sanitize("  spaced  "), "spaced")
        self.assertEqual(BRIDGE.sanitize("x" * 100, 10), "x" * 10)


class ParseRequestTest(unittest.TestCase):
    def test_plain_request(self):
        self.assertEqual(BRIDGE.parse_request(b"$REQ#\r"), (None, 0))

    def test_need_and_sort(self):
        self.assertEqual(
            BRIDGE.parse_request(b"$REQ,need=proc,sort=1#\n"), ("proc", 1)
        )

    def test_need_without_sort(self):
        self.assertEqual(BRIDGE.parse_request(b"$REQ,need=port#"), ("port", 0))
        self.assertEqual(BRIDGE.parse_request(b"$REQ,need=gpu#"), ("gpu", 0))

    def test_non_request_lines(self):
        self.assertIsNone(BRIDGE.parse_request(b"I (123) boot: ready\r"))
        self.assertIsNone(BRIDGE.parse_request(b""))


class FrameTest(unittest.TestCase):
    def test_status_frame(self):
        self.assertEqual(
            BRIDGE.build_frame([("cpu", "12.5"), ("mem", "40.0")]),
            b"$PCSTAT,cpu=12.5,mem=40.0#\n",
        )

    def test_agent_frame_reports_psutil(self):
        with patch.object(BRIDGE, "psutil", object()):
            self.assertEqual(
                BRIDGE.build_agent_frame(), b"$PCAGENT,ver=4,psutil=1#\n"
            )
        with patch.object(BRIDGE, "psutil", None):
            self.assertEqual(
                BRIDGE.build_agent_frame(), b"$PCAGENT,ver=4,psutil=0#\n"
            )

    def test_proc_frame(self):
        frame = BRIDGE.build_proc_frame([("chrome", 12.3, 400.0)], 1)
        self.assertEqual(frame, b"$PCPROC,by=1,cnt=1,p0=chrome,u0=12.3,m0=400#\n")

    def test_port_frame(self):
        frame = BRIDGE.build_port_frame([(8000, "python")], 3)
        self.assertEqual(frame, b"$PCPORT,cnt=1,est=3,t0=8000,n0=python#\n")

    def test_gpu_frame(self):
        frame = BRIDGE.build_gpu_frame([("v", "nvidia"), ("u", "23.0")])
        self.assertEqual(frame, b"$PCGPU,v=nvidia,u=23.0#\n")


class DetectPortTest(unittest.TestCase):
    def _detect(self, ports, explicit=None):
        with patch.object(BRIDGE.list_ports, "comports", return_value=ports):
            return BRIDGE.detect_port(explicit)

    def test_explicit_port_wins(self):
        self.assertEqual(self._detect([], "COM42"), "COM42")

    def test_prefers_esp_vid(self):
        ports = [_FakePort("COM3"), _FakePort("COM7", vid=BRIDGE.ESP_VID)]
        self.assertEqual(self._detect(ports), "COM7")

    def test_single_port_is_used(self):
        self.assertEqual(self._detect([_FakePort("COM9")]), "COM9")

    def test_no_port_raises(self):
        with self.assertRaises(BRIDGE.PortUnavailable):
            self._detect([])

    def test_multiple_ports_raise(self):
        ports = [_FakePort("COM1"), _FakePort("COM2")]
        with self.assertRaises(BRIDGE.PortUnavailable):
            self._detect(ports)


class SingleInstanceTest(unittest.TestCase):
    def test_second_acquire_is_rejected_then_released(self):
        with tempfile.TemporaryDirectory(prefix="pcmon-bridge-tests-") as directory:
            path = str(Path(directory) / "pcmon-bridge.lock")
            first = BRIDGE.SingleInstance(path).acquire()
            try:
                with self.assertRaises(BRIDGE.AlreadyRunning):
                    BRIDGE.SingleInstance(path).acquire()
            finally:
                first.release()
            again = BRIDGE.SingleInstance(path).acquire()
            again.release()


class StatePathTest(unittest.TestCase):
    def test_default_paths(self):
        self.assertEqual(Path(BRIDGE.default_log_path()).name, "pcmon-bridge.log")
        self.assertEqual(Path(BRIDGE.default_lock_path()).name, "pcmon-bridge.lock")
        self.assertIn("FoloToy", BRIDGE.default_log_path())


if __name__ == "__main__":
    unittest.main(verbosity=2)
