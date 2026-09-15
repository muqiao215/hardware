#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <example-name> <serial-port>" >&2
  echo "Example: $0 01_blink /dev/ttyUSB0" >&2
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXAMPLE="$1"
PORT="$2"
HEX_FILE="${ROOT_DIR}/build/${EXAMPLE}/firmware.hex"
STCGAL_BIN="${ROOT_DIR}/.venv/bin/stcgal"

if [[ ! -f "${HEX_FILE}" ]]; then
  echo "Missing firmware hex: ${HEX_FILE}" >&2
  echo "Run: ${ROOT_DIR}/scripts/build.sh ${EXAMPLE}" >&2
  exit 1
fi

if [[ -x "${STCGAL_BIN}" ]]; then
  :
elif command -v stcgal >/dev/null 2>&1; then
  STCGAL_BIN="stcgal"
else
  echo "toolchain_pending: stcgal is required for STC flashing" >&2
  echo "Install with: python3 -m venv mcs51/.venv && mcs51/.venv/bin/pip install stcgal" >&2
  exit 1
fi

echo "hardware_pending: connect STC89C52 board, enter bootloader by power cycling when prompted"
"${STCGAL_BIN}" -P stc89 -p "${PORT}" "${HEX_FILE}"
