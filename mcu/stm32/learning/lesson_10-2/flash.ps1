param([string]$App = "servo", [string]$Board = "stm32f103c8t6")

# 1. Ensure Toolchain and OpenOCD are in PATH
$ArmGccPath = "C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin"
$OpenOCDPath = "C:\OpenOCD"

if (Test-Path $ArmGccPath) {
    $env:Path = "$ArmGccPath;$env:Path"
}
if (Test-Path "$OpenOCDPath\bin") {
    $env:Path = "$OpenOCDPath\bin;$env:Path"
}

# Verify tools
if (-not (Get-Command "arm-none-eabi-gcc" -ErrorAction SilentlyContinue)) {
    Write-Error "Error: arm-none-eabi-gcc not found in PATH or standard location."
    exit 1
}
if (-not (Get-Command "ninja" -ErrorAction SilentlyContinue)) {
    Write-Warning "Ninja not found in PATH. CMake might fail if it defaults to Ninja generator."
}

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

# Find the compiled ELF file
$ElfFile = Get-ChildItem "build/*.elf" | Select-Object -First 1
if (-not $ElfFile) {
    Write-Error "Build successful but no .elf file found in build directory."
    exit 1
}
Write-Host "Found ELF file: $($ElfFile.FullName)"

& $OpenOCDExe -s "$OpenOCDScriptDir" `
    -f "boards/$Board/openocd.cfg" `
    -c "program '$($ElfFile.FullName)' verify reset exit"
