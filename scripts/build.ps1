# ==============================================================================
# Build Script for ESP-IDF Firmware
# ==============================================================================
param (
    [string]$Target = "",
    [switch]$Clean
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
Write-Host " [Vibe Embedded] Firmware Build Pipeline " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# ESP-IDF 툴체인 환경 확인
if (-not (Get-Command idf.py -ErrorAction SilentlyContinue)) {
    Write-Host "[ERROR] 'idf.py'를 찾을 수 없습니다." -ForegroundColor Red
    exit 1
}

if ($Clean) {
    Write-Host "--> Cleaning build directory..." -ForegroundColor Yellow
    idf.py fullclean
}

if ($Target -ne "") {
    Write-Host "--> Setting target to: $Target" -ForegroundColor Green
    idf.py set-target $Target
}

Write-Host "--> Starting build..." -ForegroundColor Green
idf.py build

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n[SUCCESS] Firmware built successfully!" -ForegroundColor Green
} else {
    Write-Host "`n[FAILED] Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
