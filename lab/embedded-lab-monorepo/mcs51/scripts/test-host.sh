#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/host"

mkdir -p "${BUILD_DIR}"

cc -std=c99 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_debounce.c" \
  "${ROOT_DIR}/common/utils/debounce.c" \
  -o "${BUILD_DIR}/test_debounce"

"${BUILD_DIR}/test_debounce"

echo "mcs51 host tests passed"
