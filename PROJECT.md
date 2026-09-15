# PROJECT.md

## Why
个人拥有多台异构硬件资产（树莓派 5、Radxa ROCK 5C、x86 J1900、东芝笔记本、主力工作站），硬件日趋分散且缺乏统一的状态可观测性和选型基准。本项目旨在构建一个专注、轻量、只读的嵌入式硬件实验台与诊断探针工具。

## User Intent
1. 提供“这块板子到底怎么了”的快速只读体检诊断，事实与推断严格分离（Evidence-based Diagnostics）。
2. 提供多板卡统一的硬件快照（JSON Snapshot）采集与状态对比（Diffing）能力。
3. 建立长期且可复现的硬件基线基准测试（CPU、内存、存储只读测速、温升与降频监控），回答实际选型定位（编译、推理、低功耗服务机）。
4. 支持本地运行以及基于 SSH 的无侵入式（Agentless）远程板卡探测。

## Non-goals
- 生产环境集群编排与配置管理（非 Ansible / Kubernetes / Fleet Manager）。
- 远程执行任意危险写命令或自动固件/系统升级。
- 侵入式客户端：绝不在目标开发板上安装后台守护服务或重型依赖。

## Constraints
- **语言与依赖**：核心代码使用 Python 3 标准库编写，免额外 pip 安装。
- **环境兼容**：同时适配 Linux ARM64（SBC、树莓派、Rockchip）与 x86_64（Intel/AMD 笔记本与工控小主机）。
- **非破坏性**：仅做只读与只测安全区，严禁对物理磁盘产生破坏性写。

## Current State
- 仓库骨架已初始化，支持本地与 SSH 远程板卡无侵入采集。
- 已连通测试 Raspberry Pi 5 (`muqiaopi`) 与 Radxa ROCK 5C (`rock-5c`)。

## Knowledge Map
- `hardwire/core/probe.py`: 只读硬件与系统传感器采集器
- `hardwire/core/evidence.py`: 事实与推断分离的数据结构
- `hardwire/core/snapshot.py`: 快照存储与双快照 Diff 比较引擎
- `hardwire/targets/ssh.py`: 基于 SSH 的免安装轻量远程采集驱动
- `hardwire/benchmarks/matrix.py`: 树莓派 5、ROCK 5C、x86 的横向选型对比矩阵
