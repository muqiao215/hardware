import json
import os
from typing import Dict, Any, Tuple
from board_doctor.core.evidence import EvidenceBundle

class SnapshotManager:
    """管理快照的序列化、持久化与两期 Diff 对比"""

    @staticmethod
    def save_snapshot(bundle: EvidenceBundle, output_path: str):
        os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(bundle.to_dict(), f, indent=2, ensure_ascii=False)

    @staticmethod
    def load_snapshot(file_path: str) -> Dict[str, Any]:
        with open(file_path, "r", encoding="utf-8") as f:
            return json.load(f)

    @staticmethod
    def diff_snapshots(snap_a: Dict[str, Any], snap_b: Dict[str, Any]) -> Dict[str, Any]:
        facts_a = {f["key"]: f["value"] for f in snap_a.get("facts", [])}
        facts_b = {f["key"]: f["value"] for f in snap_b.get("facts", [])}

        diffs = {
            "machine_id_a": snap_a.get("machine_id"),
            "machine_id_b": snap_b.get("machine_id"),
            "time_a": snap_a.get("collected_at"),
            "time_b": snap_b.get("collected_at"),
            "deltas": {},
            "state_changes": []
        }

        # 1. 温度差异对比
        if "soc_temp_celsius" in facts_a and "soc_temp_celsius" in facts_b:
            ta = float(facts_a["soc_temp_celsius"])
            tb = float(facts_b["soc_temp_celsius"])
            diffs["deltas"]["temp_celsius"] = {
                "before": ta,
                "after": tb,
                "delta": round(tb - ta, 1)
            }

        # 2. 内存使用率差异
        if "used_percent" in facts_a and "used_percent" in facts_b:
            ma = float(facts_a["used_percent"])
            mb = float(facts_b["used_percent"])
            diffs["deltas"]["memory_used_percent"] = {
                "before": ma,
                "after": mb,
                "delta": round(mb - ma, 1)
            }

        # 3. 根磁盘占用对比
        if "root_use_percent" in facts_a and "root_use_percent" in facts_b:
            ra = facts_a["root_use_percent"]
            rb = facts_b["root_use_percent"]
            if ra != rb:
                diffs["state_changes"].append({
                    "metric": "root_storage_percent",
                    "before": ra,
                    "after": rb
                })

        # 4. 内核版本变更
        ka = facts_a.get("kernel_release")
        kb = facts_b.get("kernel_release")
        if ka and kb and ka != kb:
            diffs["state_changes"].append({
                "metric": "kernel_release",
                "before": ka,
                "after": kb
            })

        # 5. 网络接口差异
        ia = set(facts_a.get("active_interfaces", []))
        ib = set(facts_b.get("active_interfaces", []))
        added_ifaces = sorted(list(ib - ia))
        removed_ifaces = sorted(list(ia - ib))
        if added_ifaces or removed_ifaces:
            diffs["state_changes"].append({
                "metric": "network_interfaces",
                "before": ", ".join(sorted(ia)) or "none",
                "after": ", ".join(sorted(ib)) or "none",
                "added": added_ifaces,
                "removed": removed_ifaces
            })

        return diffs
