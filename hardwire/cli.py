import sys
import argparse
import json
import os
from hardwire.targets.local import LocalCollector
from hardwire.targets.ssh import SSHCollector
from hardwire.core.reporter import Reporter
from hardwire.core.snapshot import SnapshotManager
from hardwire.benchmarks.runner import BenchmarkRunner
from hardwire.benchmarks.matrix import BoardMatrix

def main():
    parser = argparse.ArgumentParser(
        prog="hardwire",
        description="🩺 Hardwire: 嵌入式板卡实验台与证据驱动只读诊断探针"
    )
    subparsers = parser.add_subparsers(dest="command", help="子命令")

    # collect 命令
    p_collect = subparsers.add_parser("collect", help="执行只读硬件探测与事实诊断")
    p_collect.add_argument("--local", action="store_true", help="探测当前本机系统")
    p_collect.add_argument("--ssh", type=str, help="通过 SSH 无侵入探测远程设备（如 muqiaopi / rock-5c）")
    p_collect.add_argument("--save", "--out", dest="save", type=str, help="将生成的快照保存为指定 JSON 文件路径")

    # diff 命令
    p_diff = subparsers.add_parser("diff", help="比对两份硬件快照差异")
    p_diff.add_argument("snapshot_a", type=str, help="基准快照文件路径 A")
    p_diff.add_argument("snapshot_b", type=str, help="对比快照文件路径 B")

    # bench 命令
    p_bench = subparsers.add_parser("bench", help="运行可复现的轻量级基线测试并记录温升")
    p_bench.add_argument("--save", type=str, help="保存基线数据为 JSON 文件")

    # matrix 命令
    subparsers.add_parser("matrix", help="查看树莓派 5、ROCK 5C、J1900、笔记本的横向选型对比矩阵")

    # ip-check 命令 (固定使用开源权威 IP.Check.Place 进行纯洁度与风控检测)
    p_ip = subparsers.add_parser("ip-check", help="运行开源权威 IP.Check.Place 检查 IP 纯洁度、欺诈分与流媒体/AI 解锁")
    p_ip.add_argument("--ssh", type=str, help="在指定的远程服务器上执行检测（如 meiren, qiaobird, racknerd-436b0c0）")

    args = parser.parse_args()

    if args.command == "collect":
        if args.ssh:
            print(f"正在无侵入探测远程板卡 [{args.ssh}]...")
            bundle = SSHCollector.collect(args.ssh)
        else:
            bundle = LocalCollector.collect()

        Reporter.print_terminal(bundle)

        if args.save:
            SnapshotManager.save_snapshot(bundle, args.save)
            print(f"✔ 快照已保存至: {args.save}")

    elif args.command == "diff":
        snap_a = SnapshotManager.load_snapshot(args.snapshot_a)
        snap_b = SnapshotManager.load_snapshot(args.snapshot_b)
        diff_res = SnapshotManager.diff_snapshots(snap_a, snap_b)
        print(Reporter.format_diff(diff_res))

    elif args.command == "bench":
        print("正在运行轻量级硬件基线测试（内存带宽、7-Zip MIPS、温升记录）...")
        rec = BenchmarkRunner.run_suite()
        print(f"\n【基线结果】耗时: {rec['duration_sec']}s, 初始温度: {rec['thermal']['start_celsius']}°C ➔ 结束温度: {rec['thermal']['end_celsius']}°C")
        print(f"内存申请: {rec['memory']['alloc_speed_mb_s']} MB/s, 内存拷贝: {rec['memory']['copy_speed_mb_s']} MB/s")
        if rec['cpu_7zip']['installed']:
            print(f"7-Zip CPU 总算力: {rec['cpu_7zip']['tot_mips']} MIPS (压缩 {rec['cpu_7zip']['compress_mips']} / 解压 {rec['cpu_7zip']['decompress_mips']})")
        
        if args.save:
            with open(args.save, "w", encoding="utf-8") as f:
                json.dump(rec, f, indent=2, ensure_ascii=False)
            print(f"✔ 基线数据已保存至: {args.save}")

    elif args.command == "matrix":
        print(BoardMatrix.render_markdown())

    elif args.command == "ip-check":
        import subprocess
        if args.ssh:
            print(f"正在远程服务器 [{args.ssh}] 上启动开源权威 IP.Check.Place 纯洁度检测...")
            subprocess.run(["ssh", "-t", args.ssh, "curl -Ls https://IP.Check.Place | bash -s -- -4"])
        else:
            print("正在本机启动开源权威 IP.Check.Place 纯洁度检测...")
            subprocess.run("curl -Ls https://IP.Check.Place | bash -s -- -4", shell=True)

    else:
        parser.print_help()

if __name__ == "__main__":
    main()

