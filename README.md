<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# PC Monitor

Turn an AI Passport device into a small desktop dashboard for the PC it is
plugged into. The device asks and the PC answers: the device periodically sends
`$REQ#` over its USB serial port, and the companion bridge replies with live
CPU, memory, disk, network and GPU status, plus the busiest processes and the
ports that are listening. Everything stays local; no network setup is required.

This repository contains the device firmware application and the PC-side bridge
that feeds it. The firmware is also distributed through the community play on
the FoloToy AI Passport platform.

## Quick start

1. Flash the firmware.
   - Use the community play, or build this branch with ESP-IDF 5.5.3.
2. Run the PC bridge (Windows quick path).
   - Get the code: `git clone -b feature/pc-status-monitor https://github.com/Bigd1gua/pc-monitor.git`
   - Bridge file: [`tools/pc_status_bridge.py`](tools/pc_status_bridge.py)
   - Install dependencies once: `pip install pyserial psutil`
   - Double-click [`tools/install_bridge.cmd`](tools/install_bridge.cmd) to install it and start it at every logon, or run `python tools\pc_status_bridge.py` directly.
   - On macOS or Linux, run `python3 tools/pc_status_bridge.py` manually (the installer scripts are Windows-only).
3. Connect.
   - Plug the device into the PC with a data-capable USB cable. The device opens the dashboard automatically.

## Controls

- Up / Down: previous / next page (wraps around).
- OK (short press): refresh immediately; on the processes page it toggles sorting by CPU or by memory.
- OK (long press): pause / resume polling (the footer shows PAUSED).

## Pages

Core, CPU, memory, disk, network, trend, system, GPU, processes, ports, and
agent status (11 pages total).

## Bridge notes

- Requirements: Python 3, `pyserial`, `psutil`. `psutil` is strongly recommended; without it only host, OS and uptime are sent.
- Options: `--port COMx` to pick a port, `--once` to print one local sample.
- GPU data needs NVIDIA `nvidia-smi`; without it the adapter model is still shown via WMI.
- Uninstall: [`tools/uninstall_bridge.cmd`](tools/uninstall_bridge.cmd) or `install_autostart.ps1 -Remove`.

## Layout

- `main/` application UI and PC-monitor logic
- `tools/pc_status_bridge.py` PC-side bridge
- `assets/fonts/` subset Chinese fonts used by the UI
- `tests/` host tests for the protocol

## License

MIT, Copyright (c) 2026 FoloToy. See [`LICENSE`](LICENSE).
