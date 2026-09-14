import subprocess
from board_doctor.core.evidence import EvidenceBundle
from board_doctor.core.probe import SystemProbe
from board_doctor.core.diagnostic import DiagnosticEngine
from board_doctor.targets.local import PROBE_SCRIPT, parse_probe_stream

class SSHCollector:
    """基于 SSH 的无侵入式（Agentless）远程板卡探测器"""

    @classmethod
    def collect(cls, ssh_target: str, timeout: int = 10) -> EvidenceBundle:
        """
        通过 SSH 在远程设备上执行只读探针脚本
        ssh_target 可以是 ~/.ssh/config 别名（如 muqiaopi、rock-5c），或者是 user@host 格式
        """
        ssh_cmd = [
            "ssh",
            "-o", "ConnectTimeout=5",
            "-o", "BatchMode=yes",
            "-o", "StrictHostKeyChecking=accept-new",
            ssh_target,
            f"bash -s"
        ]

        proc = subprocess.Popen(
            ssh_cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        try:
            stdout, stderr = proc.communicate(input=PROBE_SCRIPT, timeout=timeout)
        except subprocess.TimeoutExpired:
            proc.kill()
            raise TimeoutError(f"连接远程设备 {ssh_target} 超时（{timeout}s）")

        if proc.returncode != 0 and not stdout:
            raise RuntimeError(f"远程设备 {ssh_target} 探测失败: {stderr.strip()}")

        raw_sections = parse_probe_stream(stdout)
        bundle = EvidenceBundle(machine_id=ssh_target, arch=raw_sections.get("arch", "unknown"))
        SystemProbe.parse_raw_data(bundle, raw_sections)
        DiagnosticEngine.run_diagnostics(bundle)
        return bundle
