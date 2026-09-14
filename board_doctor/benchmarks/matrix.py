from typing import Dict, List, Any

# 典型嵌入式板卡与小主机硬件事实基线
HARDWARE_DATABASE = [
    {
        "id": "rpi5",
        "name": "Raspberry Pi 5",
        "soc": "Broadcom BCM2712",
        "arch": "ARM64 (v8.2-A)",
        "cores": "4 × Cortex-A76 @ 2.4GHz",
        "ram": "4GB / 8GB LPDDR4X-4267",
        "npu": "无 (可选 PCIe 外接微型算力卡)",
        "pcie": "PCIe 2.0 / 3.0 x1",
        "storage": "MicroSD + M.2 NVMe HAT",
        "idle_power": "~2.8W",
        "load_power": "~10.5W",
        "best_for": {
            "service": "⭐⭐⭐⭐☆ (社区生态最强，树莓派 OS 极其稳定)",
            "compile": "⭐⭐⭐☆☆ (4核 A76 够用，但大型 C++ 编译慢于 8核)",
            "inference": "⭐⭐☆☆☆ (纯 CPU 推理，跑轻量视觉尚可，大模型吃力)",
            "power_save": "⭐⭐⭐⭐☆ (低负载功耗控制出色，但无待机开关机制)"
        },
        "verdict": "适合作为主力家庭中枢服务、网络服务、自动化网关（社区资料最全）。"
    },
    {
        "id": "rock5c",
        "name": "Radxa ROCK 5C",
        "soc": "Rockchip RK3588S",
        "arch": "ARM64 (v8.2-A)",
        "cores": "4 × A76 (2.4GHz) + 4 × A55 (1.8GHz) 大小核",
        "ram": "4GB / 8GB / 16GB LPDDR4X",
        "npu": "6 TOPS 独立 NPU (支持 INT4/INT8/FP16)",
        "pcie": "PCIe 2.1 M.2 M-Key",
        "storage": "MicroSD / eMMC 5.1 / M.2 NVMe",
        "idle_power": "~2.5W",
        "load_power": "~12.0W",
        "best_for": {
            "service": "⭐⭐⭐⭐☆ (稳定性已随 6.x 主线内核日趋成熟)",
            "compile": "⭐⭐⭐⭐☆ (8 核心全开，多线程编译速度明显领先 Pi 5)",
            "inference": "⭐⭐⭐⭐⭐ (板载 6 TOPS NPU，RKNN 跑 YOLOv8/bilingual 极佳)",
            "power_save": "⭐⭐⭐⭐☆ (小核 A55 待机仅两瓦出头，能效比极高)"
        },
        "verdict": "嵌入式算力怪兽：最适合边缘模型推理（RKNN）、视频编解码和多任务编译。"
    },
    {
        "id": "j1900",
        "name": "Intel J1900 工控小主机",
        "soc": "Intel Celeron J1900",
        "arch": "x86_64 (Silvermont)",
        "cores": "4 × 4线程 @ 2.0~2.4GHz",
        "ram": "DDR3L 1333 (常见 4G/8G)",
        "npu": "无",
        "pcie": "PCIe 2.0 (多网口板卡扩展)",
        "storage": "SATA / mSATA 盘",
        "idle_power": "~7.0W",
        "load_power": "~15.0W",
        "best_for": {
            "service": "⭐⭐⭐⭐☆ (经典 x86 软路由、OpenWrt/PVE 双软路由老将)",
            "compile": "⭐⭐☆☆☆ (单核性能老旧，缺失 AVX 指令集，现代编译慢)",
            "inference": "⭐☆☆☆☆ (不支持现代神经网络指令，推理慢)",
            "power_save": "⭐⭐⭐☆☆ (低发热无风扇，但功耗相比 ARM 架构偏高)"
        },
        "verdict": "适合作为全千兆物理软路由、旁路网关或简单 Samba 本地存储机。"
    },
    {
        "id": "laptop_linux",
        "name": "x86 笔记本 (如东芝/蛟龙)",
        "soc": "标压/低压 x86 处理器",
        "arch": "x86_64 (带 AVX2 / AVX-512)",
        "cores": "8~16 线程",
        "ram": "16G ~ 32G 高速内存",
        "npu": "可选独立显卡 CUDA",
        "pcie": "PCIe 3.0 / 4.0 高速通道",
        "storage": "高速 NVMe SSD",
        "idle_power": "~10W ~ 20W",
        "load_power": "~45W ~ 100W",
        "best_for": {
            "service": "⭐⭐⭐☆☆ (自带电池相当于自带 UPS，不怕偶发断电)",
            "compile": "⭐⭐⭐⭐⭐ (x86 高主频 + 大内存，编译速度呈压倒性优势)",
            "inference": "⭐⭐⭐⭐⭐ (搭配独显 CUDA，可跑大模型与深度学习微调)",
            "power_save": "⭐⭐☆☆☆ (不适合 24 小时低成本无看管挂机)"
        },
        "verdict": "重型研发工作站：负责编译出包、模型蒸馏调试，不适合当常开小网关。"
    }
]

class BoardMatrix:
    """多板卡选型与硬件横向定位对比看板"""

    @classmethod
    def render_markdown(cls) -> str:
        md = []
        md.append("# 嵌入式板卡与小主机横向对比矩阵 (Board Matrix)\n")
        md.append("重点解决：**在手头这几块板子中，到底哪个做服务、哪个做编译、哪个做推理、哪个最省电？**\n")
        
        # 表格一：核心硬件规格
        md.append("### 1. 硬件参数与芯片底色")
        md.append("| 板卡名称 | SoC 架构 | 核心规格 | 内存上限 | NPU 算力 | 存储接口 | 待机/满载功耗 |")
        md.append("|---|---|---|---|---|---|---|")
        for b in HARDWARE_DATABASE:
            md.append(f"| **{b['name']}** | {b['arch']} | {b['cores']} | {b['ram']} | {b['npu']} | {b['storage']} | {b['idle_power']} / {b['load_power']} |")

        md.append("\n### 2. 真实场景适用性评分 (满分五星)")
        md.append("| 板卡名称 | 长期低功耗服务机 | 软硬件代码编译 | 边缘模型推理 | 节能省电/温控 | 核心定性总结 |")
        md.append("|---|---|---|---|---|---|")
        for b in HARDWARE_DATABASE:
            md.append(f"| **{b['name']}** | {b['best_for']['service']} | {b['best_for']['compile']} | {b['best_for']['inference']} | {b['best_for']['power_save']} | {b['verdict']} |")

        md.append("\n### 3. 选型决策指南 (Answer to Practical Questions)\n")
        md.append("1. **哪个适合作为长期服务机？**")
        md.append("   - **首选：Raspberry Pi 5**。生态极其稳健，各类 Docker 镜像和外设驱动支持最好，排错成本最低。")
        md.append("   - **次选：东芝/老款笔记本**。如果部署必须保证断电不崩，老笔记本自带的锂电池天然就是免维护 UPS。\n")

        md.append("2. **哪个适合作为编译机？**")
        md.append("   - **ARM 生态首选：Radxa ROCK 5C**。4大核 + 4小核（8线程）协同工作，编译速度比 Pi 5 明显快一个量级。")
        md.append("   - **全平台性能首选：x86 主力笔记本**。主频高，NVMe 写入快，内存大，适合重型大包构建。\n")

        md.append("3. **哪个适合模型推理？**")
        md.append("   - **边缘板载绝对首选：Radxa ROCK 5C**。自带的 6 TOPS NPU 是树莓派 5 完全不具备的原生硬件优势，通过 RKNN 框架跑 YOLOv8 视觉检测或轻量语言模型极快。")
        md.append("   - **通用大模型推理：带 CUDA 独显的笔记本**。\n")

        md.append("4. **哪个最省电？**")
        md.append("   - **待机省电之王：Radxa ROCK 5C**（A55 能效小核待机约 2W）与 **Raspberry Pi 5**（约 2.8W）。按 24 小时开机计算，一年电费仅约 10~15 元。")

        return "\n".join(md)
