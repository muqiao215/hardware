import os
import time
import subprocess
import json
from typing import Dict, Any

class BenchmarkRunner:
    """可复现、保存原始测试上下文（硬件、内核、温度变化、降频）的基准测试器"""

    @classmethod
    def read_temp(cls) -> float:
        try:
            with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
                val = int(f.read().strip())
                return round(val / 1000.0, 1) if val > 1000 else float(val)
        except Exception:
            return 0.0

    @classmethod
    def run_memory_bench(cls, size_mb: int = 256) -> Dict[str, Any]:
        """纯 Python 快速内存块申请与拷贝带宽测速 (MB/s)"""
        nbytes = size_mb * 1024 * 1024
        t0 = time.perf_counter()
        data = bytearray(nbytes)
        t1 = time.perf_counter()
        _copy = bytearray(data)
        t2 = time.perf_counter()

        alloc_time = max(t1 - t0, 0.0001)
        copy_time = max(t2 - t1, 0.0001)

        alloc_speed = round(size_mb / alloc_time, 2)
        copy_speed = round(size_mb / copy_time, 2)

        return {
            "buffer_size_mb": size_mb,
            "alloc_speed_mb_s": alloc_speed,
            "copy_speed_mb_s": copy_speed
        }

    @classmethod
    def run_7zip_bench(cls) -> Dict[str, Any]:
        """若系统安装了 7z，运行 1 轮轻量 7z b 并解析多核 MIPS 得分 (限制字典为 16 以节省内存与时间)"""
        result = {"installed": False, "tot_mips": 0, "compress_mips": 0, "decompress_mips": 0}
        try:
            out = subprocess.check_output(["7z", "b", "-md=16", "1"], stderr=subprocess.DEVNULL, timeout=45).decode()
            result["installed"] = True
            for line in out.splitlines():
                if "Tot:" in line:
                    parts = line.split()
                    if len(parts) >= 4:
                        result["tot_mips"] = int(parts[-1])
                elif "Avr:" in line:
                    subparts = line.split("|")
                    if len(subparts) == 2:
                        left = subparts[0].split()
                        right = subparts[1].split()
                        if left:
                            result["compress_mips"] = int(left[-1])
                        if right:
                            result["decompress_mips"] = int(right[-1])
        except Exception:
            pass
        return result

    @classmethod
    def run_suite(cls) -> Dict[str, Any]:
        temp_start = cls.read_temp()
        t0 = time.time()

        # 内存吞吐
        mem_res = cls.run_memory_bench(size_mb=256)

        # 7-Zip CPU 压测
        cpu_res = cls.run_7zip_bench()

        temp_end = cls.read_temp()
        elapsed = round(time.time() - t0, 2)

        record = {
            "timestamp": time.time(),
            "kernel": os.uname().release,
            "arch": os.uname().machine,
            "duration_sec": elapsed,
            "thermal": {
                "start_celsius": temp_start,
                "end_celsius": temp_end,
                "delta_celsius": round(temp_end - temp_start, 1)
            },
            "memory": mem_res,
            "cpu_7zip": cpu_res
        }
        return record
