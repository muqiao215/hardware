#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT_DIR}"

echo "==> Unit tests"
python3 -m unittest discover -s tests -v

echo "==> Packaging install smoke"
VENV_DIR="${ROOT_DIR}/.venv-check"
python3 -m venv "${VENV_DIR}"
"${VENV_DIR}/bin/python" -m pip install --upgrade pip >/dev/null
"${VENV_DIR}/bin/python" -m pip install -e .

echo "==> CLI monitor smoke"
"${VENV_DIR}/bin/hpad" monitor --backend mock --samples 2 --interval 0.0

echo "==> Logger smoke"
tmp_csv="${ROOT_DIR}/data/check_output.csv"
"${VENV_DIR}/bin/hpad" log --backend replay --csv "${ROOT_DIR}/data/sample_replay.csv" --output "${tmp_csv}" --samples 2 --interval 0.0
test -f "${tmp_csv}"
rm -f "${tmp_csv}"

echo "==> Status smoke"
"${VENV_DIR}/bin/hpad" status --backend replay --csv "${ROOT_DIR}/data/sample_replay.csv"

echo "==> Replay smoke"
"${VENV_DIR}/bin/hpad" replay --csv "${ROOT_DIR}/data/sample_replay.csv" --samples 2 --interval 0.0 --loop false

echo "==> Real backend honesty smoke"
set +e
real_status_output="$("${VENV_DIR}/bin/hpad" status --backend real 2>&1)"
real_status_code=$?
set -e
test "${real_status_code}" -eq 2
printf '%s\n' "${real_status_output}" | grep 'backend_unavailable' >/dev/null

echo "==> ROS2 fallback smoke"
"${VENV_DIR}/bin/python" -m ros2_pkg.analog_io_node --backend replay --csv "${ROOT_DIR}/data/sample_replay.csv" --samples 2

echo "rpi_hpad check passed"
