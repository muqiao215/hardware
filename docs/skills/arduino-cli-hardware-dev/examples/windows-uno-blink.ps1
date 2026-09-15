param(
  [Parameter(Mandatory = $true)]
  [string]$Port,

  [string]$Fqbn = "arduino:avr:uno",
  [string]$ConfigFile = ".\\.arduino-cli\\arduino-cli.yaml",
  [string]$SketchDir = ".\\sketches\\Blink"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
  throw "arduino-cli not found in PATH"
}

arduino-cli --config-file $ConfigFile core update-index
arduino-cli --config-file $ConfigFile core install arduino:avr
arduino-cli --config-file $ConfigFile compile -b $Fqbn $SketchDir
arduino-cli --config-file $ConfigFile upload -p $Port -b $Fqbn $SketchDir

Write-Host "Done. Optional serial monitor:"
Write-Host "  arduino-cli --config-file `"$ConfigFile`" monitor -p `"$Port`" --describe"
Write-Host "  arduino-cli --config-file `"$ConfigFile`" monitor -p `"$Port`" -c baudrate=115200"

