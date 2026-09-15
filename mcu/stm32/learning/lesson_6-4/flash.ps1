param([string]$App = "servo", [string]$Board = "stm32f103c8t6")

$toolchain = "cmake/toolchains/arm-gcc.cmake"
# Determine the project root or relative path specific to where this is run
# Assuming this script is run from the lesson folder (e.g., lesson_6-4)
# And cmake/toolchains is relative to that?
# Wait, user said: cmake -S . -B build ... -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake
# This implies the toolchain file IS inside the lesson folder OR the user has a common layout.
# Looking at the `sembedded` repo usually, it might have a structure.
# I will use the user's exact suggested content.

cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=$toolchain -DBOARD=$Board -DAPP=$App
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build build
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# OpenOCD command
# Path configuration based on the installed xPack OpenOCD
$OpenOCDPath = "C:\OpenOCD"
$OpenOCDScriptDir = "$OpenOCDPath\openocd\scripts"
$OpenOCDExe = "$OpenOCDPath\openocd.exe"

# Double check if scripts dir exists, if not try 'share' variant
if (-not (Test-Path $OpenOCDScriptDir)) {
    if (Test-Path "$OpenOCDPath\share\openocd\scripts") {
        $OpenOCDScriptDir = "$OpenOCDPath\share\openocd\scripts"
    }
}

# Check if openocd is in PATH, if not use explicit path
if (Get-Command "openocd" -ErrorAction SilentlyContinue) {
    # Even if in path, we might need explicit scripts path if not standard
    # But let's prefer the explicit one we just found
    Write-Host "Using OpenOCD from PATH or configured location..."
}

Write-Host "Using OpenOCD: $OpenOCDExe"
Write-Host "Using Scripts: $OpenOCDScriptDir"

& $OpenOCDExe -s "$OpenOCDScriptDir" `
    -f "boards/$Board/openocd.cfg" `
    -c "program build/firmware.elf verify reset exit"
