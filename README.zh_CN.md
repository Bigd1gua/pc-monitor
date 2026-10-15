<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 电脑状态监控

把 AI Passport 变成插在电脑旁的迷你仪表盘：设备发出请求，电脑来回答。
设备通过 USB 串口周期发送 `$REQ#`，电脑端 Bridge 回传实时的 CPU、内存、磁盘、
网络与显卡状态，以及占用最高的进程和正在监听的端口。数据只在本机读取，
无需配网。

本仓库包含设备固件应用与喂给它的电脑端 Bridge。固件也通过 FoloToy AI
Passport 社区的“电脑状态监控”玩法分发。

## 快速开始

1. 烧录固件。
   - 在社区玩法中直接烧录，或用 ESP-IDF 5.5.3 构建本分支。
2. 运行电脑端 Bridge（Windows 一键路径）。
   - 获取代码：`git clone -b feature/pc-status-monitor https://github.com/Bigd1gua/pc-monitor.git`
   - Bridge 文件：[`tools/pc_status_bridge.py`](tools/pc_status_bridge.py)
   - 先安装依赖：`pip install pyserial psutil`
   - 双击 [`tools/install_bridge.cmd`](tools/install_bridge.cmd) 一键安装并设为登录自启；也可以直接运行 `python tools\pc_status_bridge.py`。
   - macOS 或 Linux 请手动运行 `python3 tools/pc_status_bridge.py`（一键脚本仅支持 Windows）。
3. 连接。
   - 用支持数据传输的 USB 线把设备连到电脑，设备会自动进入仪表盘。

## 按键操作

- 上 / 下：上/下一页（循环）。
- OK 短按：立即刷新；在“进程”页可在“按 CPU 排序”和“按内存排序”之间切换。
- OK 长按：暂停 / 恢复轮询（底部显示 PAUSED）。

## 页面

核心、CPU、内存、磁盘、网络、趋势、系统、显卡、进程、端口、代理状态，共 11 页。

## Bridge 说明

- 依赖：Python 3、`pyserial`、`psutil`。强烈建议安装 `psutil`；缺少时只发送主机、系统与运行时长。
- 参数：`--port COMx` 指定串口，`--once` 打印一次本机采样。
- 显卡数据需要 NVIDIA `nvidia-smi`；缺少时仍通过 WMI 显示型号。
- 卸载：[`tools/uninstall_bridge.cmd`](tools/uninstall_bridge.cmd) 或 `install_autostart.ps1 -Remove`。

## 目录

- `main/` 应用界面与监控逻辑
- `tools/pc_status_bridge.py` 电脑端 Bridge
- `assets/fonts/` 界面使用的中文子集字体
- `tests/` 协议主机测试

## 许可

MIT，Copyright (c) 2026 FoloToy。见 [`LICENSE`](LICENSE)。
