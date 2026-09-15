from typing import List, Optional
from hardwire.core.evidence import EvidenceBundle, Fact

class DiagnosticEngine:
    """证据驱动的只读诊断引擎。严格区分事实与推断，附带边界声明（Caveat）"""

    @classmethod
    def run_diagnostics(cls, bundle: EvidenceBundle) -> EvidenceBundle:
        facts_by_key = {f.key: f for f in bundle.facts}

        # 1. 温控与节流诊断
        temp_fact = facts_by_key.get("soc_temp_celsius")
        if temp_fact:
            temp = temp_fact.value
            if temp >= 80.0:
                bundle.add_inference(
                    id="THERMAL_HIGH_TEMPERATURE",
                    level="WARNING",
                    claim=f"SoC 结温偏高 ({temp}°C)，处于可能降频的临界区域",
                    evidence=[temp_fact],
                    caveat="温度高仅反映热量堆积或散热片接触不足，不能直接推断物理节流已触发，需结合 cpufreq 实际频率判定。"
                )
            elif temp >= 65.0:
                bundle.add_inference(
                    id="THERMAL_WARM_TEMPERATURE",
                    level="NOTICE",
                    claim=f"SoC 结温处于中负载工况 ({temp}°C)",
                    evidence=[temp_fact],
                    caveat="无降频风险，但密闭弱散热环境下建议关注持续负载温升。"
                )
            else:
                bundle.add_inference(
                    id="THERMAL_NOMINAL",
                    level="INFO",
                    claim=f"SoC 温控非常优良 ({temp}°C)，被动或主动散热处于健康状态",
                    evidence=[temp_fact],
                    caveat="仅代表当前轻载/待机温度，全核编译或 NPU 满载时需复测。"
                )

        # 2. 根文件系统余量诊断
        root_pct_fact = facts_by_key.get("root_use_percent")
        root_avail_fact = facts_by_key.get("root_avail")
        if root_pct_fact:
            try:
                pct = int(root_pct_fact.value.replace("%", ""))
                evidence = [root_pct_fact]
                if root_avail_fact:
                    evidence.append(root_avail_fact)

                if pct >= 90:
                    bundle.add_inference(
                        id="STORAGE_ROOT_CRITICAL",
                        level="CRITICAL",
                        claim=f"根目录空间极度告急 (已占用 {pct}%)，系统随时可能写入受阻",
                        evidence=evidence,
                        caveat="仅代表文件系统空间配额，不能据此推断存储介质的 SMART 寿命或物理寿命。"
                    )
                elif pct >= 80:
                    bundle.add_inference(
                        id="STORAGE_ROOT_WARNING",
                        level="WARNING",
                        claim=f"根分区使用率较高 ({pct}%)，需关注日志与缓存积累",
                        evidence=evidence,
                        caveat="仅反映逻辑使用率，不代表介质存在坏道。"
                    )
                else:
                    bundle.add_inference(
                        id="STORAGE_ROOT_HEALTHY",
                        level="INFO",
                        claim=f"根文件系统余量充足 (占用率 {pct}%)",
                        evidence=evidence,
                        caveat="存储被成功识别并挂载不代表其掉电保护安全，关键数据仍需异机冷备。"
                    )
            except ValueError:
                pass

        # 3. 内存与 Swap 交换压力
        mem_used_pct = facts_by_key.get("used_percent")
        swap_used_mb = facts_by_key.get("swap_used_mb")
        swap_total_mb = facts_by_key.get("swap_total_mb")
        if mem_used_pct:
            evidence = [mem_used_pct]
            if swap_used_mb:
                evidence.append(swap_used_mb)
            if swap_total_mb:
                evidence.append(swap_total_mb)

            used = mem_used_pct.value
            swap_u = swap_used_mb.value if swap_used_mb else 0
            if used >= 90.0 and swap_u > 500:
                bundle.add_inference(
                    id="MEMORY_PRESSURE_CRITICAL",
                    level="WARNING",
                    claim=f"物理内存极度紧张 (使用率 {used}%) 且正在大量借助 Swap ({swap_u} MB)，系统可能出现卡顿",
                    evidence=evidence,
                    caveat="高内存占用是当前运行态特征，不能直接推断是内存泄漏还是合理的服务缓存行为。"
                )
            else:
                bundle.add_inference(
                    id="MEMORY_HEALTHY",
                    level="INFO",
                    claim=f"系统内存余量充足 (物理内存占用率 {used}%)",
                    evidence=evidence,
                    caveat="内存余量健康不保证瞬时大并发或大模型加载时不会引发 OOM 保护。"
                )

        # 4. 网络介质连通性推断
        ifaces_fact = facts_by_key.get("active_interfaces")
        if ifaces_fact:
            ifaces = ifaces_fact.value
            bundle.add_inference(
                id="NETWORK_LINK_DETECTED",
                level="INFO",
                claim=f"网络协议栈已建立并探测到可用网卡接口: {', '.join(ifaces)}",
                evidence=[ifaces_fact],
                caveat="【严谨性声明】网卡链路建立仅代表网线插好或 WiFi 关联成功，不能直接推断出外网连通性或 DNS 可靠性。"
            )

        # 5. 内核日志异常分析
        dmesg_fact = facts_by_key.get("anomaly_lines")
        if dmesg_fact:
            lines = dmesg_fact.value
            bundle.add_inference(
                id="KERNEL_ANOMALIES_PRESENT",
                level="NOTICE",
                claim=f"内核日志中捕捉到 {len(lines)} 行警告或硬件错误信息",
                evidence=[dmesg_fact],
                caveat="部分驱动模块在冷启动时打印的握手失败属于临时重试，需结合实际功能验证是否为致命硬件故障。"
            )
        else:
            bundle.add_inference(
                id="KERNEL_CLEAN",
                level="INFO",
                claim="只读内核日志安全子集中未见电源电压不足（Undervoltage）或存储 I/O 硬件级坏道报警",
                evidence=[],
                caveat="dmesg ring-buffer 轮转可能冲掉早期引导事件，零报错不等于绝对无偶发故障。"
            )

        return bundle
