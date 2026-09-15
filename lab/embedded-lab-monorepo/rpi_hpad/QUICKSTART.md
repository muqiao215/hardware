# 5-Minute Quickstart

This is the shortest path to confirm the software-only toolchain works.

## 1. Create a virtual environment

```bash
cd /home/qiaopai/rpi_hpad
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -e .
```

## 2. Discover the tool

```bash
hpad --help
```

You should see `monitor`, `log`, `replay`, and `status`.

## 3. Run the mock backend

```bash
hpad monitor --backend mock --samples 3 --interval 0.0
```

You should see JSON `status` and `sample` events.

## 4. Record a capture

```bash
hpad log --backend mock --output data/out.csv --samples 3 --interval 0.0
```

You should get a `log_complete` event and a CSV file at `data/out.csv`.

## 5. Replay the same capture

```bash
hpad replay --csv data/out.csv --samples 3 --interval 0.0 --loop false
```

You should see the same file played back as JSON `status` and `sample` events.

## 6. Check the real backend boundary

```bash
hpad status --backend real
```

Expected result:

- exit code `2`
- JSON event `backend_unavailable`
- stable code `real_backend_not_implemented`

This is the current honest boundary. Real hardware integration is not claimed in this version.
