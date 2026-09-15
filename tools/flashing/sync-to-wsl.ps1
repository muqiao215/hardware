<#
Sync this Windows folder into WSL2 (Ubuntu-22.04) using rsync.

Default:
  Windows source:  E:\web\embedded   (this script's directory)
  WSL dest:        /home/muqiao/dev/embedded
  Mode:            mirror (uses --delete)

Examples:
  pwsh -NoProfile -File .\sync-to-wsl.ps1
  pwsh -NoProfile -File .\sync-to-wsl.ps1 -DryRun
  pwsh -NoProfile -File .\sync-to-wsl.ps1 -NoDelete
  pwsh -NoProfile -File .\sync-to-wsl.ps1 -Distro Ubuntu-22.04 -Dest /home/muqiao/dev/embedded

Notes:
  - Requires rsync inside WSL: `sudo apt-get update && sudo apt-get install -y rsync`
  - WSL can access E: as /mnt/e
#>

[CmdletBinding()]
param(
  [string]$Distro = "Ubuntu-22.04",
  [string]$Source = (Split-Path -Parent $PSCommandPath),
  [string]$Dest = "/home/muqiao/dev/embedded",
  [switch]$DryRun,
  [switch]$NoDelete,
  [switch]$Progress,
  [switch]$NoProgress
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Convert-WinPathToWsl([string]$Path) {
  $full = (Resolve-Path -LiteralPath $Path).Path
  if ($full -match '^([A-Za-z]):\\(.*)$') {
    $drive = $matches[1].ToLowerInvariant()
    $rest = $matches[2] -replace '\\', '/'
    return "/mnt/$drive/$rest"
  }
  throw "Unsupported path format: $full"
}

function Invoke-WslBash([string]$BashCommand) {
  & wsl.exe -d $Distro -- bash -lc $BashCommand
  if ($LASTEXITCODE -ne 0) { throw "WSL command failed with exit code $LASTEXITCODE" }
}

function BashQuote([string]$Value) {
  # Single-quote for bash, escaping embedded single quotes safely.
  $replacement = "'" + '"' + "'" + '"' + "'"
  return "'" + ($Value -replace "'", $replacement) + "'"
}

Write-Host "WSL Sync" -ForegroundColor Cyan
Write-Host "  Distro:  $Distro"
Write-Host "  Source:  $Source"
Write-Host "  Dest:    $Dest"

$srcWsl = Convert-WinPathToWsl $Source

Write-Host "  Source (WSL): $srcWsl" -ForegroundColor DarkGray

Invoke-WslBash "command -v rsync >/dev/null 2>&1 || { echo 'ERROR: rsync not found in WSL. Install: sudo apt-get update && sudo apt-get install -y rsync' >&2; exit 2; }"

$deleteFlag = if ($NoDelete) { "" } else { "--delete" }
$dryFlag = if ($DryRun) { "--dry-run" } else { "" }
$infoFlag =
  if ($Progress -and -not $NoProgress) { "--info=progress2,stats2" }
  else { "--info=stats2" }

if (-not $NoDelete) {
  Write-Host "  Mode: mirror (--delete enabled)" -ForegroundColor Yellow
} else {
  Write-Host "  Mode: copy (no deletes)" -ForegroundColor Yellow
}
if ($DryRun) {
  Write-Host "  DryRun: yes (no changes written)" -ForegroundColor Yellow
}

# Ensure destination exists and sync.
$srcWithSlash = ($srcWsl.TrimEnd('/') + '/')
$dstWithSlash = ($Dest.TrimEnd('/') + '/')

$srcQ = BashQuote $srcWithSlash
$dstQ = BashQuote $dstWithSlash
$dstDirQ = BashQuote $Dest

$bash = @'
set -euo pipefail
mkdir -p __DSTDIR__
rsync -a --human-readable __INFO__ __DELETE__ __DRY__ __SRC__ __DST__
'@

$bash = $bash.Replace("__SRC__", $srcQ)
$bash = $bash.Replace("__DST__", $dstQ)
$bash = $bash.Replace("__DSTDIR__", $dstDirQ)
$bash = $bash.Replace("__INFO__", $infoFlag)
$bash = $bash.Replace("__DELETE__", $deleteFlag)
$bash = $bash.Replace("__DRY__", $dryFlag)

Invoke-WslBash $bash

Write-Host "Done." -ForegroundColor Green
