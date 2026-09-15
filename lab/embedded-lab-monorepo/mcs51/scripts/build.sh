#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <example-name>" >&2
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXAMPLE="$1"
EXAMPLE_DIR="${ROOT_DIR}/examples/${EXAMPLE}"
BUILD_DIR="${ROOT_DIR}/build/${EXAMPLE}"
MAIN_C="${EXAMPLE_DIR}/main.c"
SOURCES_FILE="${EXAMPLE_DIR}/sources.txt"

if [[ ! -d "${EXAMPLE_DIR}" ]]; then
  echo "Unknown MCS-51 example: ${EXAMPLE}" >&2
  exit 1
fi

if [[ ! -f "${MAIN_C}" ]]; then
  echo "Missing example main.c: ${MAIN_C}" >&2
  exit 1
fi

if ! command -v sdcc >/dev/null 2>&1 || ! command -v packihx >/dev/null 2>&1; then
  echo "toolchain_pending: sdcc and packihx are required" >&2
  echo "Install with: sudo apt install sdcc" >&2
  exit 1
fi

mkdir -p "${BUILD_DIR}"

sources=()

if [[ -f "${SOURCES_FILE}" ]]; then
  while IFS= read -r source || [[ -n "${source}" ]]; do
    [[ -z "${source}" ]] && continue
    sources+=("${EXAMPLE_DIR}/${source}")
  done < "${SOURCES_FILE}"
else
  sources+=("${MAIN_C}")
fi

compile_flags=(
  -mmcs51
  --std-sdcc99
  --model-small
  -I"${ROOT_DIR}/boards/stc89c52rc"
  -I"${ROOT_DIR}/common/core"
  -I"${ROOT_DIR}/common/drivers"
  -I"${ROOT_DIR}/common/utils"
  -I"${EXAMPLE_DIR}"
)

objects=()

for source in "${sources[@]}"; do
  object="${BUILD_DIR}/$(basename "${source%.c}").rel"
  sdcc "${compile_flags[@]}" -c -o "${object}" "${source}"
  objects+=("${object}")
done

sdcc "${compile_flags[@]}" \
  --out-fmt-ihx \
  -o "${BUILD_DIR}/firmware.ihx" \
  "${objects[@]}"

packihx "${BUILD_DIR}/firmware.ihx" > "${BUILD_DIR}/firmware.hex"

printf 'built %s\n' "${BUILD_DIR}/firmware.hex"
