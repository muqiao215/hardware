# MCS-51 / STC89C52

这里收纳的是 51 单片机学习资料入口，当前先以 `STC89C52` 课程/源码资料为主。

当前定位：

- 这是独立于 STM32 主线的 51 迁移线
- 当前 Phase 1 已打通最小 CLI 构建链路
- 迁移计划见 `plans/mcs51_migration/`

当前收录上游：

- `Learn_STC89C52`

目录：

- `mcs51/Learn_STC89C52/`
  上游课程仓库快照
- `mcs51/UPSTREAM.md`
  上游来源和导入信息

说明：

- 当前 STM32F103 裸机构建、`scripts/check.sh`、validation bundle 仍然只覆盖 STM32 主线
- STC89C52 后续会使用独立的 `mcs51/scripts/` 和 `mcs51/examples/`，不复用 STM32 的 CMake/ST-Link 假设
- Phase 1 先用 shell 脚本和 SDCC 原生薄头文件，不上 CMake，不写厚 `REGX52.H` 兼容层
- 当前已验证 `01_blink` 可在树莓派上从 `.c -> .ihx -> .hex`
- 51 资料现在的价值主要是：
  - 统一收口到同一个学习仓库
  - 保留课程源码与章节结构
  - 为后续单独做 8051/STC CLI 迁移提供来源

Phase 1 命令：

```bash
sudo apt install sdcc
python3 -m venv mcs51/.venv
mcs51/.venv/bin/pip install stcgal
mcs51/scripts/probe-toolchain.sh
mcs51/scripts/build.sh 01_blink
mcs51/scripts/check.sh
```

当前状态：

- `sdcc` / `packihx`：已验证可用
- `stcgal`：默认走 `mcs51/.venv/bin/stcgal`
- `01_blink`：已验证可生成 `firmware.ihx` 和 `firmware.hex`
- 真板烧录与现象验证：仍为 `hardware_pending`
