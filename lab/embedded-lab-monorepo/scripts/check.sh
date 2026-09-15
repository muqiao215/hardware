#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ROOT="${ROOT_DIR}/build"

mapfile -t examples < <(find "${ROOT_DIR}/examples" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

echo "==> Full build"
"${ROOT_DIR}/scripts/build-all.sh"

echo "==> Host-side tests"
"${ROOT_DIR}/scripts/test-host.sh"

echo "==> Structure checks"
test -d "${ROOT_DIR}/boards/stm32f103c8t6"
test -d "${ROOT_DIR}/common/core"
test -d "${ROOT_DIR}/common/drivers"
test -d "${ROOT_DIR}/common/components"
test -d "${ROOT_DIR}/common/cli"
test -d "${ROOT_DIR}/common/utils"
test -f "${ROOT_DIR}/CHANGELOG.md"
test -f "${ROOT_DIR}/RELEASE_NOTES.md"
test -f "${ROOT_DIR}/HARDWARE_PENDING.md"
test -f "${ROOT_DIR}/COMPATIBILITY.md"
test -f "${ROOT_DIR}/REALBOARD_VALIDATION_CHECKLIST.md"
test -f "${ROOT_DIR}/VERSION"
test -f "${ROOT_DIR}/scripts/package-validation-bundle.sh"

echo "==> Example file checks"
for example in "${examples[@]}"; do
  test -f "${ROOT_DIR}/examples/${example}/README.md"
  test -f "${ROOT_DIR}/examples/${example}/CMakeLists.txt"
  [[ "${example}" =~ ^[0-9][0-9]_ ]]
done

echo "==> Artifact checks"
for example in "${examples[@]}"; do
  test -f "${BUILD_ROOT}/${example}/firmware.elf"
  test -f "${BUILD_ROOT}/${example}/firmware.hex"
  test -f "${BUILD_ROOT}/${example}/firmware.bin"
  test -f "${BUILD_ROOT}/${example}/firmware.map"
done

echo "==> CMake contract checks"
if grep -R "common/bsp" -n "${ROOT_DIR}/examples"/*/CMakeLists.txt >/dev/null 2>&1; then
  echo "Found legacy common/bsp references in example CMake files" >&2
  exit 1
fi

if grep -R "STD_PERIPH\\|BSP" -n "${ROOT_DIR}/examples"/*/CMakeLists.txt >/dev/null 2>&1; then
  echo "Found legacy BSP/STD_PERIPH source enumeration in example CMake files" >&2
  exit 1
fi

echo "==> Size summary"
for example in "${examples[@]}"; do
  printf '%s\n' "-- ${example}"
  arm-none-eabi-size --format=berkeley "${BUILD_ROOT}/${example}/firmware.elf"
done

echo "==> Validation bundle"
BUNDLE_DIR="$("${ROOT_DIR}/scripts/package-validation-bundle.sh")"
test -d "${BUNDLE_DIR}"
test -f "${BUNDLE_DIR}/INDEX.md"
for example in "${examples[@]}"; do
  test -f "${BUNDLE_DIR}/${example}/README.md"
  test -f "${BUNDLE_DIR}/${example}/VALIDATION.md"
  test -f "${BUNDLE_DIR}/${example}/METADATA.txt"
  test -f "${BUNDLE_DIR}/${example}/firmware.elf"
  test -f "${BUNDLE_DIR}/${example}/firmware.hex"
  test -f "${BUNDLE_DIR}/${example}/firmware.bin"
  test -f "${BUNDLE_DIR}/${example}/firmware.map"
done

echo "check.sh: all no-hardware gates passed"
