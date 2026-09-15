#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
missing=0

probe() {
  local tool="$1"
  local hint="$2"
  local version_cmd="${3-__USE_DEFAULT__}"

  if [[ "${version_cmd}" == "__USE_DEFAULT__" ]]; then
    version_cmd="--version"
  fi

  if command -v "${tool}" >/dev/null 2>&1; then
    if [[ -n "${version_cmd}" ]]; then
      printf '%s: %s\n' "${tool}" "$("${tool}" ${version_cmd} 2>&1 | sed -n '1p')"
    else
      printf '%s: present (%s)\n' "${tool}" "$(command -v "${tool}")"
    fi
  else
    printf '%s: missing (%s)\n' "${tool}" "${hint}" >&2
    missing=1
  fi
}

probe sdcc "install with: sudo apt install sdcc"
probe packihx "usually provided by the sdcc package" ""

STCGAL_BIN="${ROOT_DIR}/.venv/bin/stcgal"

if [[ -x "${STCGAL_BIN}" ]]; then
  printf 'stcgal: %s (%s)\n' "$("${STCGAL_BIN}" --version 2>&1 | sed -n '1p')" "${STCGAL_BIN}"
elif command -v stcgal >/dev/null 2>&1; then
  printf 'stcgal: %s\n' "$(stcgal --version 2>&1 | sed -n '1p')"
else
  printf 'stcgal: missing (install with: python3 -m venv mcs51/.venv && mcs51/.venv/bin/pip install stcgal)\n' >&2
  missing=1
fi

if [[ "${missing}" -ne 0 ]]; then
  printf 'toolchain_pending\n' >&2
  exit 1
fi

printf 'mcs51 toolchain ready\n'
