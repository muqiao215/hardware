#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "==> STM32F103 gate"
"${ROOT_DIR}/scripts/check.sh"

echo "==> MCS-51 gate"
"${ROOT_DIR}/mcs51/scripts/check.sh"

echo "==> RPi HPAD gate"
"${ROOT_DIR}/rpi_hpad/scripts/check.sh"

echo "all monorepo checks passed"
