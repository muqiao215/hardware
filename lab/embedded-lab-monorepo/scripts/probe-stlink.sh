#!/usr/bin/env bash
set -euo pipefail

if ! command -v st-info >/dev/null 2>&1; then
  echo "st-info not found. Install stlink tools first." >&2
  exit 1
fi

st-info --probe
