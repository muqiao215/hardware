from typing import Dict, Any
from hardwire.core.evidence import EvidenceBundle

C_RESET  = "\033[0m"
C_BOLD   = "\033[1m"
C_CYAN   = "\033[36m"
C_GREEN  = "\033[32m"
C_YELLOW = "\033[33m"
C_RED    = "\033[31m"
C_BLUE   = "\033[34m"

class Reporter:
    """格式化输出报告：终端高亮卡片与 Markdown 格式"""

    @classmethod
    def print_terminal(cls, bundle: EvidenceBundle):
        facts = {f.key: f.value for f in bundle.facts}

        print(f"\n{C_BOLD}{C_CYAN}╔══════════════════════════════════════════════════════════════════════╗{C_RESET}")
        print(f"{C_BOLD}{C_CYAN}║             🩺 Hardwire · 硬件只读体检与事实诊断报告             ║{C_RESET}")
        print(f"{C_BOLD}{C_CYAN}╚══════════════════════════════════════════════════════════════════════╝{C_RESET}")

        # 硬件事实摘要
        board = facts.get("board_model", "Unknown")
        host = facts.get("hostname", "Unknown")
        cpu = facts.get("model_name", "Unknown")
        threads = facts.get("threads", 1)
        mem_tot = facts.get("total_mb", 0)
        mem_avail = facts.get("available_mb", 0)
        temp = facts.get("soc_temp_celsius", "N/A")
        kernel = facts.get("kernel_release", "Unknown")
        root_dev = facts.get("root_device", "Unknown")
        root_use = facts.get("root_use_percent", "Unknown")

        print(f"  {C_BOLD}【目标主机】{C_RESET} {C_GREEN}{host}{C_RESET} ({board})")
        print(f"  {C_BOLD}【系统内核】{C_RESET} Linux {kernel} ({bundle.arch})")
        print(f"  {C_BOLD}【处理器拓扑】{C_RESET} {C_YELLOW}{cpu}{C_RESET} ({threads} 线程/核心)")
        print(f"  {C_BOLD}【内存容量】{C_RESET} {mem_tot} MB (当前可用: {mem_avail} MB)")
        print(f"  {C_BOLD}【根存储空间】{C_RESET} {root_dev} (占用率: {root_use})")
        print(f"  {C_BOLD}【当前温度】{C_RESET} {temp}°C")

        # 诊断与推断结论 (Facts vs Inferences)
        print(f"\n{C_BOLD}{C_BLUE}─── 📋 事实推断与诊断结论 (Evidence vs Inference) ───{C_RESET}")
        for inf in bundle.inferences:
            color = C_GREEN if inf.level == "INFO" else (C_YELLOW if inf.level in ("NOTICE", "WARNING") else C_RED)
            icon = "✔" if inf.level == "INFO" else ("⚠" if inf.level in ("NOTICE", "WARNING") else "✖")
            print(f"\n  {color}{C_BOLD}[{inf.level}] {icon} {inf.claim}{C_RESET}")
            if inf.evidence:
                ev_str = ", ".join([f"{e['source']}:{e['key']}={e['value']}" for e in inf.evidence])
                print(f"      {C_BOLD}底层证据:{C_RESET} {ev_str}")
            if inf.caveat:
                print(f"      {C_YELLOW}边界声明:{C_RESET} {inf.caveat}")

        print(f"{C_BOLD}{C_CYAN}════════════════════════════════════════════════════════════════════════{C_RESET}\n")

    @classmethod
    def format_diff(cls, diff: Dict[str, Any]) -> str:
        lines = []
        lines.append(f"\n{C_BOLD}{C_CYAN}╔══════════════════════════════════════════════════════════════════════╗{C_RESET}")
        lines.append(f"{C_BOLD}{C_CYAN}║             📊 Hardwire · 硬件快照差异对比报告 (Diff)            ║{C_RESET}")
        lines.append(f"{C_BOLD}{C_CYAN}╚══════════════════════════════════════════════════════════════════════╝{C_RESET}")
        lines.append(f"基准快照 A: {diff.get('machine_id_a')}")
        lines.append(f"对比快照 B: {diff.get('machine_id_b')}")

        deltas = diff.get("deltas", {})
        if deltas:
            lines.append(f"\n{C_BOLD}【数值指标变化】{C_RESET}")
            for k, v in deltas.items():
                d = v['delta']
                sign = "+" if d > 0 else ""
                color = C_YELLOW if abs(d) > 5 else C_GREEN
                lines.append(f"  • {k}: {v['before']} ➔ {v['after']} ({color}{sign}{d}{C_RESET})")

        changes = diff.get("state_changes", [])
        if changes:
            lines.append(f"\n{C_BOLD}【状态/环境变更】{C_RESET}")
            for c in changes:
                lines.append(f"  • {c['metric']}: {c.get('before')} ➔ {c.get('after')}")

        lines.append(f"{C_BOLD}{C_CYAN}════════════════════════════════════════════════════════════════════════{C_RESET}\n")
        return "\n".join(lines)
