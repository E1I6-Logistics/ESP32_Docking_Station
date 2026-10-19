# ==============================================================================
# Flash and Monitor Script for ESP-IDF Firmware
# ==============================================================================
param (
    [string]$Port = "",
    [int]$Baud = 115200,
    [switch]$BuildFirst,
    [switch]$MonitorOnly,
    [switch]$FlashOnly
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Split-Path -Parent $ScriptDir

# ESP-IDF 환경 자동 로드
if (Test-Path "$ScriptDir/Idf-Env.ps1") {
    . "$ScriptDir/Idf-Env.ps1"
    Import-EspIdfEnv
}

Set-Location $ProjectDir

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " [Vibe Embedded] Flash & Monitor Pipeline " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# 포트 자동 탐색
if ([string]::IsNullOrWhiteSpace($Port)) {
    if (Test-Path "$ScriptDir/Detect-Port.ps1") {
        $detectedPort = & "$ScriptDir/Detect-Port.ps1"
        if ($detectedPort) {
            $Port = $detectedPort.Trim()
            Write-Host "--> Auto-detected target port: $Port" -ForegroundColor Green
        }
    }
}

if ([string]::IsNullOrWhiteSpace($Port)) {
    Write-Host "[ERROR] 사용 가능한 직렬 포트를 찾을 수 없습니다. ESP32 보드가 연결되어 있는지 확인하거나 -Port COMx 를 지정하세요." -ForegroundColor Red
    exit 1
}

# 빌드 선행 옵션
if ($BuildFirst) {
    Write-Host "--> Building firmware first..." -ForegroundColor Green
    & "$ScriptDir/build.ps1"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[ERROR] 빌드에 실패하여 플래시를 중단합니다." -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

$pythonExe = "C:\Espressif\python_env\idf5.3_py3.11_env\Scripts\python.exe"
$esptoolPy = "C:\Espressif\frameworks\esp-idf-v5.3\components\esptool_py\esptool\esptool.py"

if (-not $MonitorOnly) {
    Write-Host ""
    Write-Host "------------------------------------------------------------" -ForegroundColor Yellow
    Write-Host " [ESP32 다운로드 모드 준비]" -ForegroundColor Yellow
    Write-Host "  1. 보드의 [BOOT] 버튼을 꾹 누르고 계세요." -ForegroundColor White
    Write-Host "  2. 누른 상태에서 [EN] 버튼을 딸깍 1번 누릅니다." -ForegroundColor White
    Write-Host "  3. [BOOT] 버튼에서 손을 뗍니다." -ForegroundColor White
    Write-Host "------------------------------------------------------------" -ForegroundColor Yellow
    Write-Host "위 단계를 완료한 후 [Enter]를 누르면 즉시 업로드됩니다..." -ForegroundColor Cyan

    # 콘솔 입력 대기 (단순 키 입력)
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

    Write-Host "--> Writing flash (keep-boot mode)..." -ForegroundColor Green
    & $pythonExe $esptoolPy `
        -p $Port `
        -b 115200 `
        --no-stub `
        --before no_reset `
        --after hard_reset `
        write_flash `
        --flash_mode dio `
        --flash_size 2MB `
        --flash_freq 40m `
        0x1000 build\bootloader\bootloader.bin `
        0x8000 build\partition_table\partition-table.bin `
        0x10000 build\vibe_firmware.bin

    if ($LASTEXITCODE -ne 0) {
        Write-Host "`n[ERROR] 플래시 쓰기에 실패했습니다. 다운로드 모드 진입(BOOT+EN)을 다시 확인하세요." -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

if (-not $FlashOnly) {
    Write-Host "`n--> Starting serial monitor on $Port (Baud: $Baud)..." -ForegroundColor Green
    Write-Host "--> (모니터 종료 단축키: Ctrl + ])`n" -ForegroundColor Yellow
    idf.py -p $Port -b $Baud monitor
}
