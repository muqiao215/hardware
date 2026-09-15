# 🩺 Hardwire (板载大夫)

> 嵌入式板卡实验台与证据驱动只读诊断探针 (SBC & x86 Hardware Doctor)

支持 **Raspberry Pi 5**、**Radxa ROCK 5C (RK3588S)**、**x86 工控小主机 (J1900)** 与 **Linux 笔记本** 的统一硬件体检、状态快照（Snapshot）、版本 Diff 与长期基线测试。

---

## 特性

- **零侵入探测（Agentless）**：远程板卡无需安装任何常驻客户端，SSH 一次性只读采集。
- **事实与推断严格分离（Evidence-based）**：不妄言硬件好坏，每个诊断结论附带 Linux 内核底层真实证据。
- **快照与 Diff 对比**：换散热片、调频、换固件前后状态一键比对。
- **多板选型对比矩阵**：横向对比树莓派 5、ROCK 5C、J1900、轻薄本在编译、边缘推理、待机功耗上的真实定位。

## 快速上手

```bash
# 1. 本地整机体检
python3 -m hardwire collect --local

# 2. 远程无侵入探测树莓派 5
python3 -m hardwire collect --ssh muqiaopi

# 3. 远程无侵入探测 Radxa ROCK 5C
python3 -m hardwire collect --ssh rock-5c

# 4. 对比两次硬件快照
python3 -m hardwire diff snapshots/rock-5c-before.json snapshots/rock-5c-after.json

# 5. 查看嵌入式多板卡选型对比横评
python3 -m hardwire matrix
```
