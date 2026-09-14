import re
import json
from typing import Dict, Any, List
from board_doctor.core.evidence import EvidenceBundle

class SystemProbe:
    """负责将底层系统原始输出现场解析为事实条目"""

    @staticmethod
    def parse_raw_data(bundle: EvidenceBundle, raw_payload: Dict[str, str]) -> EvidenceBundle:
        # 1. 机器型号与架构
        arch = raw_payload.get("arch", "").strip() or "unknown"
        bundle.arch = arch

        model = raw_payload.get("model", "").strip()
        if not model or model == "Generic Board":
            model = raw_payload.get("dmi_product", "").strip() or "Generic Linux Machine"
        bundle.add_fact("system", "dmi/device-tree", "board_model", model)

        hostname = raw_payload.get("hostname", "").strip()
        bundle.machine_id = hostname or "unknown-machine"
        bundle.add_fact("system", "uname", "hostname", hostname)

        kernel = raw_payload.get("kernel", "").strip()
        bundle.add_fact("system", "uname", "kernel_release", kernel)

        os_release = raw_payload.get("os_release", "").strip()
        bundle.add_fact("system", "/etc/os-release", "os_pretty_name", os_release)

        # 2. CPU
        cpuname_raw = raw_payload.get("cpuname", "").strip()
        cpuinfo = raw_payload.get("cpuinfo", "")
        kernel = raw_payload.get("kernel", "").strip()

        # 针对 ARM SBC 特别精准识别 SoC 型号
        if "0xd0b" in cpuinfo and "0xd05" in cpuinfo:
            cpuname = "Rockchip RK3588/S (4×Cortex-A76 + 4×Cortex-A55)"
        elif "0xd0b" in cpuinfo and ("Raspberry" in model or "2712" in kernel):
            cpuname = "Broadcom BCM2712 (4×Cortex-A76)"
        elif "0xd08" in cpuinfo:
            cpuname = "ARM Cortex-A72"
        elif "0xd03" in cpuinfo:
            cpuname = "ARM Cortex-A53"
        elif cpuname_raw:
            lines = [l.strip() for l in cpuname_raw.splitlines() if l.strip()]
            unique_cpus = list(dict.fromkeys(lines))
            if len(unique_cpus) > 1:
                cpuname = " + ".join(unique_cpus)
            elif unique_cpus:
                cpuname = unique_cpus[0]
            else:
                cpuname = f"Generic {arch} CPU"
        else:
            for line in cpuinfo.splitlines():
                if "model name" in line or "Model" in line or "Hardware" in line:
                    parts = line.split(":", 1)
                    if len(parts) > 1:
                        cpuname = parts[1].strip()
                        break
            if not cpuname:
                cpuname = f"Generic {arch} CPU"

        bundle.add_fact("cpu", "lscpu/cpuinfo", "model_name", cpuname)

        nproc_str = raw_payload.get("nproc", "").strip()
        try:
            threads = int(nproc_str)
        except ValueError:
            threads = 1
        bundle.add_fact("cpu", "nproc", "threads", threads)

        # 3. 内存
        meminfo = raw_payload.get("meminfo", "")
        mem_total_kb = 0
        mem_avail_kb = 0
        swap_total_kb = 0
        swap_free_kb = 0
        for line in meminfo.splitlines():
            if line.startswith("MemTotal:"):
                mem_total_kb = int(line.split()[1])
            elif line.startswith("MemAvailable:"):
                mem_avail_kb = int(line.split()[1])
            elif line.startswith("SwapTotal:"):
                swap_total_kb = int(line.split()[1])
            elif line.startswith("SwapFree:"):
                swap_free_kb = int(line.split()[1])

        bundle.add_fact("memory", "/proc/meminfo", "total_mb", round(mem_total_kb / 1024, 1))
        bundle.add_fact("memory", "/proc/meminfo", "available_mb", round(mem_avail_kb / 1024, 1))
        bundle.add_fact("memory", "/proc/meminfo", "used_percent", 
                        round(100.0 * (mem_total_kb - mem_avail_kb) / mem_total_kb, 1) if mem_total_kb else 0)
        bundle.add_fact("memory", "/proc/meminfo", "swap_total_mb", round(swap_total_kb / 1024, 1))
        bundle.add_fact("memory", "/proc/meminfo", "swap_used_mb", round((swap_total_kb - swap_free_kb) / 1024, 1))

        # 4. 温度传感器
        thermal_str = raw_payload.get("thermal", "").strip()
        temp_c = None
        for line in thermal_str.splitlines():
            line = line.strip()
            if line.isdigit():
                val = int(line)
                temp_c = round(val / 1000.0, 1) if val > 1000 else float(val)
                break
            elif "temp=" in line:
                m = re.search(r"temp=([\d\.]+)", line)
                if m:
                    temp_c = float(m.group(1))
                    break
        if temp_c is not None:
            bundle.add_fact("thermal", "/sys/class/thermal", "soc_temp_celsius", temp_c)

        # 5. 存储与磁盘 (lsblk)
        lsblk_json = raw_payload.get("lsblk_json", "")
        if lsblk_json:
            try:
                data = json.loads(lsblk_json)
                devices = data.get("blockdevices", [])
                bundle.add_fact("storage", "lsblk", "blockdevices", devices)
            except Exception:
                bundle.add_fact("storage", "lsblk", "blockdevices_raw", raw_payload.get("lsblk_raw", ""))
        else:
            bundle.add_fact("storage", "lsblk", "blockdevices_raw", raw_payload.get("lsblk_raw", ""))

        df_root = raw_payload.get("df_root", "")
        for line in df_root.splitlines():
            if line.strip().startswith("/") and not line.strip().startswith("Filesystem"):
                parts = line.split()
                if len(parts) >= 5:
                    bundle.add_fact("storage", "df", "root_device", parts[0])
                    bundle.add_fact("storage", "df", "root_size", parts[1])
                    bundle.add_fact("storage", "df", "root_used", parts[2])
                    bundle.add_fact("storage", "df", "root_avail", parts[3])
                    bundle.add_fact("storage", "df", "root_use_percent", parts[4])

        # 6. 网络接口与 IP
        ip_a = raw_payload.get("ip_a", "")
        interfaces = []
        for line in ip_a.splitlines():
            m = re.match(r"^\d+:\s+([^:]+):", line)
            if m:
                iface = m.group(1).split("@")[0].strip()
                if iface != "lo":
                    interfaces.append(iface)
        bundle.add_fact("network", "ip_a", "active_interfaces", interfaces)

        # 7. dmesg 关键内核异常过滤（只读安全子集）
        dmesg_snippets = raw_payload.get("dmesg_errors", "").strip()
        if dmesg_snippets:
            bundle.add_fact("boot", "dmesg", "anomaly_lines", dmesg_snippets.splitlines())

        return bundle
