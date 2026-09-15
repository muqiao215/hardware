#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mapfile -t examples < <(find "${ROOT_DIR}/examples" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

if [[ "${#examples[@]}" -eq 0 ]]; then
  echo "No MCS-51 examples found" >&2
  exit 1
fi

for example in "${examples[@]}"; do
  echo "==> Building ${example}"
  "${ROOT_DIR}/scripts/build.sh" "${example}"
done
