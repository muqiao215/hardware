# System Architecture

## Overview
`hardwire` 是面向 Linux 嵌入式单板计算机（SBC）及小型 x86 主机的轻量只读诊断与硬件基准套件。架构坚持**无侵入（Agentless）**、**证据链（Evidence-based）**与**确定性快照（Reproducible Snapshots）**。

## Architecture Diagram
```
              ┌──────────────────────────────────────────────┐
              │           hardwire CLI / Entry           │
              └──────────────┬───────────────────────────────┘
                             │
            ┌────────────────┴────────────────┐
            ▼                                 ▼
   ┌──────────────────┐              ┌──────────────────┐
   │  Local Target    │              │   SSH Target     │
   │  (/sys, /proc)   │              │ (Agentless Run)  │
   └────────┬─────────┘              └────────┬─────────┘
            │                                 │
            └────────────────┬────────────────┘
                             ▼
              ┌──────────────────────────────┐
              │    Unified Collector Core    │
              │  (CPU, Mem, Disks, Sensors)  │
              └──────────────┬───────────────┘
                             ▼
              ┌──────────────────────────────┐
              │   Evidence vs Inference      │
              │     Diagnostic Engine        │
              └──────────────┬───────────────┘
                             ▼
        ┌────────────────────┴───────────────────┐
        ▼                                        ▼
┌──────────────────┐                    ┌──────────────────┐
│  JSON Snapshot   │ ◄── [Diff Engine] ─►  Terminal / MD   │
│ (snapshots/*.json│                    │ Diagnostic Sheet │
└──────────────────┘                    └──────────────────┘
```

## Core Invariants
1. **零写危险（Zero Dangerous Writes）**：不写入任何持久存储的关键扇区；基准测试仅测 CPU 整数/内存带宽或限制在 `/tmp` 小型测试文件。
2. **证据闭环（Evidence Traceability）**：每一项健康度结论、报警或诊断，都对应着至少一个原始原始事实字典（Raw Fact）。
3. **无缝多架构（Multi-Arch Native）**：同时支持 AArch64 设备树（`/proc/device-tree`）与 x86 DMI/SMBIOS（`/sys/class/dmi`）。
