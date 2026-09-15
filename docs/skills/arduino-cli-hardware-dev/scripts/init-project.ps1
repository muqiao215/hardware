param(
  [string]$Root = ".",
  [string]$ProjectName = "arduino-project",
  [switch]$Force
)

$ErrorActionPreference = "Stop"

function New-Dir([string]$Path) {
  if (-not (Test-Path -LiteralPath $Path)) {
    New-Item -ItemType Directory -Path $Path | Out-Null
  }
}

$rootPath = Resolve-Path -LiteralPath $Root
$projectPath = Join-Path $rootPath $ProjectName

if ((Test-Path -LiteralPath $projectPath) -and -not $Force) {
  throw "Project path already exists: $projectPath (use -Force to continue)"
}

New-Dir $projectPath
New-Dir (Join-Path $projectPath ".arduino-cli")
New-Dir (Join-Path $projectPath ".arduino-cli\\data")
New-Dir (Join-Path $projectPath ".arduino-cli\\staging")
New-Dir (Join-Path $projectPath ".arduino-cli\\user")
New-Dir (Join-Path $projectPath "sketches")
New-Dir (Join-Path $projectPath "sketches\\Blink")

$configPath = Join-Path $projectPath ".arduino-cli\\arduino-cli.yaml"
$blinkPath = Join-Path $projectPath "sketches\\Blink\\Blink.ino"

if ((Test-Path -LiteralPath $configPath) -and -not $Force) {
  throw "Config already exists: $configPath (use -Force to overwrite)"
}

@"
board_manager:
  additional_urls: []

directories:
  data: ./.arduino-cli/data
  downloads: ./.arduino-cli/staging
  user: ./.arduino-cli/user

output:
  no_color: true
"@ | Set-Content -LiteralPath $configPath -Encoding UTF8

if ((Test-Path -LiteralPath $blinkPath) -and -not $Force) {
  throw "Sketch already exists: $blinkPath (use -Force to overwrite)"
}

@"
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
"@ | Set-Content -LiteralPath $blinkPath -Encoding UTF8

$gitignorePath = Join-Path $projectPath ".gitignore"
if (-not (Test-Path -LiteralPath $gitignorePath) -or $Force) {
  @"
.arduino-cli/data/
.arduino-cli/staging/
.arduino-cli/user/
"@ | Set-Content -LiteralPath $gitignorePath -Encoding UTF8
}

Write-Host "Initialized: $projectPath"
Write-Host "Next:"
Write-Host "  cd `"$projectPath`""
Write-Host "  arduino-cli --config-file .\\.arduino-cli\\arduino-cli.yaml core update-index"
Write-Host "  arduino-cli --config-file .\\.arduino-cli\\arduino-cli.yaml board list --discovery-timeout 10s"
Write-Host "  arduino-cli --config-file .\\.arduino-cli\\arduino-cli.yaml compile -b <FQBN> .\\sketches\\Blink"
Write-Host "  arduino-cli --config-file .\\.arduino-cli\\arduino-cli.yaml upload -p <PORT> -b <FQBN> .\\sketches\\Blink"

