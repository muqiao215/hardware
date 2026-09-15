import subprocess
from typing import Dict
from hardwire.core.evidence import EvidenceBundle
from hardwire.core.probe import SystemProbe
from hardwire.core.diagnostic import DiagnosticEngine

PROBE_SCRIPT = r"""
echo "===ARCH==="
uname -m
echo "===HOSTNAME==="
hostname
echo "===KERNEL==="
uname -r
echo "===OS_RELEASE==="
cat /etc/os-release 2>/dev/null | grep PRETTY_NAME | cut -d= -f2 | tr -d '"'
echo "===MODEL==="
if [ -f /proc/device-tree/model ]; then
    cat /proc/device-tree/model
    echo ""
elif [ -f /sys/class/dmi/id/product_name ]; then
    cat /sys/class/dmi/id/product_name
    echo ""
else
    echo "Generic Board"
fi
echo "===CPUNAME==="
lscpu 2>/dev/null | grep -i "Model name:" | cut -d: -f2 | sed 's/^[ \t]*//'
echo "===CPUINFO==="
cat /proc/cpuinfo
echo "===NPROC==="
nproc
echo "===MEMINFO==="
cat /proc/meminfo
echo "===THERMAL==="
cat /sys/class/thermal/thermal_zone0/temp 2>/dev/null || cat /sys/class/hwmon/hwmon*/temp1_input 2>/dev/null | head -n 1 || vcgencmd measure_temp 2>/dev/null || nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader 2>/dev/null
echo "===LSBLK_RAW==="
lsblk -o NAME,SIZE,TYPE,MOUNTPOINT,MODEL 2>/dev/null
echo "===LSBLK_JSON==="
lsblk -J -o NAME,SIZE,TYPE,MOUNTPOINT,MODEL 2>/dev/null
echo "===DF_ROOT==="
df -h /
echo "===IP_A==="
ip a 2>/dev/null
echo "===DMESG_ERRORS==="
dmesg 2>/dev/null | grep -iE "error|fail|undervoltage|throttle|corrupt" | tail -n 10
"""

def parse_probe_stream(output: str) -> Dict[str, str]:
    sections = {}
    current_key = None
    buffer = []

    for line in output.splitlines():
        if line.startswith("===") and line.endswith("==="):
            if current_key:
                sections[current_key] = "\n".join(buffer).strip()
            current_key = line.replace("===", "").strip().lower()
            buffer = []
        else:
            buffer.append(line)

    if current_key:
        sections[current_key] = "\n".join(buffer).strip()

    return sections

class LocalCollector:
    """采集当前本机系统的只读硬件与健康指标"""

    @classmethod
    def collect(cls) -> EvidenceBundle:
        res = subprocess.run(["bash", "-c", PROBE_SCRIPT], capture_output=True, text=True)
        raw_sections = parse_probe_stream(res.stdout)
        bundle = EvidenceBundle(machine_id="local", arch=raw_sections.get("arch", "unknown"))
        SystemProbe.parse_raw_data(bundle, raw_sections)
        DiagnosticEngine.run_diagnostics(bundle)
        return bundle
