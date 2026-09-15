#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "==> MCS-51 toolchain probe"
"${ROOT_DIR}/scripts/probe-toolchain.sh"

echo "==> MCS-51 full build"
"${ROOT_DIR}/scripts/build-all.sh"

echo "==> MCS-51 host-side tests"
"${ROOT_DIR}/scripts/test-host.sh"

echo "==> MCS-51 artifact checks"
mapfile -t examples < <(find "${ROOT_DIR}/examples" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

for example in "${examples[@]}"; do
  test -f "${ROOT_DIR}/examples/${example}/README.md"
  test -f "${ROOT_DIR}/examples/${example}/main.c"
  test -f "${ROOT_DIR}/build/${example}/firmware.ihx"
  test -f "${ROOT_DIR}/build/${example}/firmware.hex"
done

echo "mcs51 check passed"
