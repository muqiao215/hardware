#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLCHAIN_FILE="${ROOT_DIR}/cmake/toolchains/arm-gcc.cmake"
BUILD_ROOT="${ROOT_DIR}/build"
mapfile -t examples < <(find "${ROOT_DIR}/examples" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

for example in "${examples[@]}"; do
  echo "==> Building ${example}"
  cmake -S "${ROOT_DIR}/examples/${example}" -B "${BUILD_ROOT}/${example}" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}"
  cmake --build "${BUILD_ROOT}/${example}"
done
