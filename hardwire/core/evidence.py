from dataclasses import dataclass, field, asdict
from typing import Any, Dict, List, Optional
import time

@dataclass
class Fact:
    """一个底层原始观察到的事实证据"""
    category: str      # e.g. cpu, memory, disk, network, thermal, boot
    source: str        # e.g. /proc/cpuinfo, lsblk, dmesg, ethtool
    key: str           # e.g. model_name, eth0_speed, root_fs_free_bytes
    value: Any         # e.g. 1000, 55.1, "/dev/nvme0n1"
    timestamp: float = field(default_factory=time.time)

@dataclass
class Inference:
    """基于一个或多个底层事实推导出的解释性诊断结论"""
    id: str
    level: str         # "INFO", "NOTICE", "WARNING", "CRITICAL"
    claim: str         # 明确推断结论，避免绝对化断言
    evidence: List[Dict[str, Any]] # 支持该推断的具体原始证据清单
    caveat: Optional[str] = None   # 明确指出该结论不能推导出的边界（免责与严谨性）

@dataclass
class EvidenceBundle:
    """证据与推断的统一载体"""
    machine_id: str
    arch: str
    collected_at: float = field(default_factory=time.time)
    facts: List[Fact] = field(default_factory=list)
    inferences: List[Inference] = field(default_factory=list)

    def add_fact(self, category: str, source: str, key: str, value: Any) -> Fact:
        f = Fact(category=category, source=source, key=key, value=value)
        self.facts.append(f)
        return f

    def add_inference(self, id: str, level: str, claim: str, evidence: List[Fact], caveat: Optional[str] = None):
        ev_dicts = [{"source": e.source, "key": e.key, "value": e.value} for e in evidence]
        inf = Inference(id=id, level=level, claim=claim, evidence=ev_dicts, caveat=caveat)
        self.inferences.append(inf)

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)
