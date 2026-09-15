#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <example-name>" >&2
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXAMPLE="$1"
BUILD_DIR="${ROOT_DIR}/build/${EXAMPLE}"
TOOLCHAIN_FILE="${ROOT_DIR}/cmake/toolchains/arm-gcc.cmake"

if [[ ! -d "${ROOT_DIR}/examples/${EXAMPLE}" ]]; then
  echo "Unknown example: ${EXAMPLE}" >&2
  exit 1
fi

cmake -S "${ROOT_DIR}/examples/${EXAMPLE}" -B "${BUILD_DIR}" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}"
cmake --build "${BUILD_DIR}" --target flash
