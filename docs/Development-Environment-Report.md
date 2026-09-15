# Development Environment Report

## Python (Windows)

- `python` / `python3` resolved to WindowsApps stubs and did not run scripts correctly.
- Use the Python Launcher instead:
  - Run scripts with `py -3 ...`
  - Example (chat export):
    - `py -3 C:\Users\11614\.codex\skills\chat-history-exporter\scripts\export_chat_history.py --days 14 --sources codex --out E:\web\embedded\_tmp\codex-embedded-14d.jsonl`

## Obsidian / Obsidian CLI

- Root cause of `Vault not found.`:
  - The vault previously referred to as `编程笔记` is no longer registered under that name in Obsidian.
  - The current registered vault name is `知识库`, path `E:\web\知识库`.
- Verified current vaults:
  - `知识库`
  - `电子电路笔记`
  - `Ops-Vault`
  - `英语词义分类数据库（大学版）（带词汇表目录）`
- Correct CLI targeting example:
  - `obsidian vault="知识库" file path="机器人与仿真/嵌入式调试/Arduino机械臂调试复盘-2026-03-02.md"`
- Obsidian was upgraded with `winget`:
  - Before: runtime `1.12.7`, installer metadata `1.12.4`
  - After: `1.12.7 (installer 1.12.7)`
- After upgrade, the CLI could not find Obsidian until the desktop app was started again.
  - Verified working after launching `C:\Users\11614\AppData\Local\Programs\Obsidian\Obsidian.exe`
