param(
  [string]$AdditionalUrls = "https://arduino.esp8266.com/stable/package_esp8266com_index.json",
  [string]$ConfigFile = ".\\.arduino-cli\\arduino-cli.yaml"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
  throw "arduino-cli not found in PATH"
}

Write-Host "Using additional Boards Manager URL:"
Write-Host "  $AdditionalUrls"

arduino-cli --config-file $ConfigFile core update-index --additional-urls $AdditionalUrls
arduino-cli --config-file $ConfigFile core search esp8266 --additional-urls $AdditionalUrls
Write-Host "Next (pick an ID from search results):"
Write-Host "  arduino-cli --config-file `"$ConfigFile`" core install esp8266:esp8266 --additional-urls `"$AdditionalUrls`""

