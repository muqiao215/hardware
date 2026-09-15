#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ROOT="${ROOT_DIR}/build"
BUNDLE_ROOT="${ROOT_DIR}/dist/validation-bundles"
TIMESTAMP="$(date '+%Y%m%d-%H%M%S')"
GIT_COMMIT="$(git -C "${ROOT_DIR}" rev-parse --short HEAD 2>/dev/null || echo unknown)"
TOOLCHAIN_VERSION="$(arm-none-eabi-gcc --version | sed -n '1p' 2>/dev/null || echo unavailable)"
STLINK_VERSION="$(st-info --version 2>/dev/null || echo unavailable)"
HOST_INFO="$(uname -a)"

if [[ ! -d "${ROOT_DIR}/examples" ]]; then
  echo "examples directory not found" >&2
  exit 1
fi

mkdir -p "${BUNDLE_ROOT}"
BUNDLE_DIR="${BUNDLE_ROOT}/${TIMESTAMP}-${GIT_COMMIT}"
mkdir -p "${BUNDLE_DIR}"

mapfile -t examples < <(find "${ROOT_DIR}/examples" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

for example in "${examples[@]}"; do
  EXAMPLE_DIR="${BUNDLE_DIR}/${example}"
  BUILD_DIR="${BUILD_ROOT}/${example}"
  README_PATH="${ROOT_DIR}/examples/${example}/README.md"
  FLASH_COMMAND="./scripts/flash.sh ${example}"

  mkdir -p "${EXAMPLE_DIR}"

  for artifact in firmware.elf firmware.hex firmware.bin firmware.map; do
    if [[ ! -f "${BUILD_DIR}/${artifact}" ]]; then
      echo "missing build artifact for ${example}: ${artifact}" >&2
      exit 1
    fi
    cp "${BUILD_DIR}/${artifact}" "${EXAMPLE_DIR}/"
  done

  cp "${README_PATH}" "${EXAMPLE_DIR}/README.md"

  cat > "${EXAMPLE_DIR}/VALIDATION.md" <<EOF
# ${example} Validation Bundle

## Flash Command

\`\`\`bash
${FLASH_COMMAND}
\`\`\`

## Included Files

- firmware.elf
- firmware.hex
- firmware.bin
- firmware.map
- README.md

## Expected Verification Workflow

1. Run \`scripts/probe-stlink.sh\`
2. Flash the example with the command above
3. Follow the wiring, expected behavior, serial output, and failure notes from \`README.md\`
4. Record whether the observed board behavior matches the README

## Status Note

This bundle was produced on a build-only host. If ST-Link is still not detected on the validation machine, hardware remains \`hardware_pending\`.
EOF

  cat > "${EXAMPLE_DIR}/METADATA.txt" <<EOF
example=${example}
git_commit=${GIT_COMMIT}
build_time=${TIMESTAMP}
toolchain=${TOOLCHAIN_VERSION}
stlink_tools=${STLINK_VERSION}
host=${HOST_INFO}
flash_command=${FLASH_COMMAND}
readme=README.md
hardware_status=hardware_pending_until_realboard_validation
EOF
done

cat > "${BUNDLE_DIR}/INDEX.md" <<EOF
# Validation Bundle Index

Generated: ${TIMESTAMP}

Git commit: \`${GIT_COMMIT}\`

Toolchain: \`${TOOLCHAIN_VERSION}\`

ST-Link tools: \`${STLINK_VERSION}\`

Host: \`${HOST_INFO}\`

Examples:
$(for example in "${examples[@]}"; do printf -- "- %s\n" "${example}"; done)

Use each example directory as a handoff package for board-side validation.
EOF

printf '%s\n' "${BUNDLE_DIR}"
