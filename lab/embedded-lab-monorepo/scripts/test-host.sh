#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/host-tests"

mkdir -p "${BUILD_DIR}"

echo "==> Host test: CLI"
gcc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/cli" \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_cli.c" \
  "${ROOT_DIR}/common/utils/cli_parser.c" \
  "${ROOT_DIR}/common/utils/cli_dispatch.c" \
  -o "${BUILD_DIR}/test_cli"
"${BUILD_DIR}/test_cli"

echo "==> Host test: CLI input pump"
gcc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/cli" \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_cli_input_pump.c" \
  "${ROOT_DIR}/common/cli/cli_input_pump.c" \
  "${ROOT_DIR}/common/utils/ringbuf.c" \
  -o "${BUILD_DIR}/test_cli_input_pump"
"${BUILD_DIR}/test_cli_input_pump"

echo "==> Host test: ringbuf"
gcc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_ringbuf.c" \
  "${ROOT_DIR}/common/utils/ringbuf.c" \
  -o "${BUILD_DIR}/test_ringbuf"
"${BUILD_DIR}/test_ringbuf"

echo "==> Host test: debounce"
gcc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_debounce.c" \
  "${ROOT_DIR}/common/utils/debounce.c" \
  -o "${BUILD_DIR}/test_debounce"
"${BUILD_DIR}/test_debounce"

echo "==> Host test: framebuffer"
gcc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/common/utils" \
  "${ROOT_DIR}/tests/host/test_framebuffer.c" \
  "${ROOT_DIR}/common/utils/framebuffer.c" \
  -o "${BUILD_DIR}/test_framebuffer"
"${BUILD_DIR}/test_framebuffer"

echo "test-host.sh: all host-side tests passed"
