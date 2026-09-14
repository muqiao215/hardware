# AGENTS.md

## Critical Rules
- **Continuity Convention**: This project follows **SpecMesh v1.1**. Read `PROJECT.md` before significant planning or changes.
- **Safety First**: All diagnostic probes and benchmarks must be strictly **read-only and non-destructive**.
- **No Fleet Control**: Do not attempt remote fleet management, automatic firmware updates, or orchestrating background daemons.
- **Evidence vs Inference**: Every diagnostic claim or anomaly flagged must link directly to verifiable raw output (e.g. `lscpu`, `dmesg`, `/sys/class/thermal`).
- **Dependencies**: Keep runtime dependencies to Python 3.10+ standard library. External tools must be optional fallbacks.
