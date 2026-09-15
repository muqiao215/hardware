$destDir = "E:\web\embedded\51_mcu\learning"

# Robust Path Resolution using Wildcards
$sourceDirObj = Resolve-Path "E:\web\embedded\51*\*\*\*\KeilProject"
if (!$sourceDirObj) {
    Write-Error "Could not find 'KeilProject' directory using wildcard pattern."
    exit
}
$sourceDir = $sourceDirObj.Path
Write-Host "Found source directory via wildcard."

# Create destination
if (!(Test-Path $destDir)) { New-Item -ItemType Directory -Path $destDir | Out-Null }

$prefixMap = @{
    "2-1" = "lesson_02_01_led_on"
    "2-2" = "lesson_02_02_led_blink"
    "2-3" = "lesson_02_03_led_flow"
    "2-4" = "lesson_02_04_led_flow_plus"
    "3-1" = "lesson_03_01_key_led_on"
    "3-2" = "lesson_03_02_key_led_state"
    "3-3" = "lesson_03_03_key_led_bin"
    "3-4" = "lesson_03_04_key_led_shift"
    "4-1" = "lesson_04_01_static_segment"
    "4-2" = "lesson_04_02_dynamic_segment"
    "5-1" = "lesson_05_01_modular_programming"
    "5-2" = "lesson_05_02_lcd1602_debug"
    "6-1" = "lesson_06_01_matrix_key"
    "6-2" = "lesson_06_02_matrix_key_lock"
    "7-1" = "lesson_07_01_key_led_flow_timer"
    "7-2" = "lesson_07_02_timer_clock"
    "8-1" = "lesson_08_01_uart_send"
    "8-2" = "lesson_08_02_uart_control_led"
    "9-1" = "lesson_09_01_led_matrix_graphic"
    "9-2" = "lesson_09_02_led_matrix_anim"
    "10-1" = "lesson_10_01_ds1302"
    "10-2" = "lesson_10_02_ds1302_adjust"
    "11-1" = "lesson_11_01_buzzer_beep"
    "11-2" = "lesson_11_02_buzzer_music"
    "12-1" = "lesson_12_01_at24c02"
    "12-2" = "lesson_12_02_stopwatch"
    "13-1" = "lesson_13_01_ds18b20"
    "13-2" = "lesson_13_02_ds18b20_alarm"
    "14-1" = "lesson_14_01_lcd1602"
    "15-1" = "lesson_15_01_led_breath"
    "15-2" = "lesson_15_02_motor_pwm"
    "16-1" = "lesson_16_01_ad"
    "16-2" = "lesson_16_02_da"
    "17-1" = "lesson_17_01_ir_remote"
    "17-2" = "lesson_17_02_ir_motor"
}

$dirs = Get-ChildItem -Path $sourceDir -Directory

foreach ($prefix in $prefixMap.Keys) {
    # Match logic: StartsWith "2-1 " OR Equals "2-1"
    $match = $dirs | Where-Object { $_.Name.StartsWith("$prefix ") -or $_.Name -eq $prefix } | Select-Object -First 1
    
    if ($match) {
        $targetName = $prefixMap[$prefix]
        $destPath = Join-Path $destDir $targetName
        Write-Host "Migrating '$prefix' to '$targetName'..."
        Copy-Item -Path $match.FullName -Destination $destPath -Recurse -Force
    } else {
        Write-Warning "No match for prefix: $prefix"
    }
}
