# PROJECT.md

## Why
个人拥有多台异构硬件与单片机资产（树莓派 5、Radxa ROCK 5C、x86 J1900、东芝笔记本、STM32、51单片机、ESP32），此前散落在多个零散仓库。本项目将所有实体硬件代码、微控制器工程（MCU）、固件烧录工具与板卡诊断探针（原 hardwire + embedded-learning）统一收敛至唯一的硬件大本营——**`hardware`**。

## User Intent
1. **纯粹面向实物硬件**：仅涵盖物理硬件与微控制器工程，与云端服务器运维（Ops-Vault）严格物理隔离。
2. **微控制器工程归集**：集中沉淀 51 单片机、STM32、ESP32 机械臂控制器及嵌入式实验台代码。
3. **硬件听诊器与只读体检**：提供快速只读体检诊断，事实与推断严格分离（Evidence-based Diagnostics）。
4. **板卡硬件基准**：建立长期可复现的板卡基线测试（CPU、内存、只读测速、温升），回答物理板卡选型定位。

## Non-goals
- 云端 VPS 舰队与在线生产服务运维（该职责 100% 归属于 `Ops-Vault`）。
- 侵入式客户端：探针绝不在目标开发板上常驻重型依赖。

## Directory Map
- `mcu/`: 单片机与控制器工程（51, STM32, ESP32）
- `lab/`: 嵌入式综合实验工作区（embedded-lab-monorepo）
- `tools/`: 烧录工具与跨端脚本（flash.ps1, migrate_51.ps1 等）
- `docs/`: 硬件接线、引脚定义与工具链指南
- `hardwire/`: 硬件体检探针与只读诊断核心模块（core, targets, benchmarks）
- `snapshots/`: 板卡硬件体检快照数据
