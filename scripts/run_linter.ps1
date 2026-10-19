# ==============================================================================
# Clang-Format Linter Script for Firmware
# ==============================================================================
param (
    [switch]$CheckOnly
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Split-Path -Parent $ScriptDir

Set-Location $ProjectDir

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " [Vibe Embedded] Code Style & Linter      " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# clang-format 탐색
$clangFormat = Get-Command clang-format -ErrorAction SilentlyContinue
if (-not $clangFormat) {
    # ESP-IDF 툴체인 디렉터리 내 검색
    $candidate = Get-ChildItem -Path "C:\Espressif" -Filter "clang-format.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($candidate) {
        $clangFormat = $candidate.FullName
    }
}

if (-not $clangFormat) {
    Write-Host "[WARNING] 'clang-format'을 찾을 수 없습니다. 포맷팅을 건너뜁니다." -ForegroundColor Yellow
    exit 0
}

$sourceFiles = Get-ChildItem -Path "$ProjectDir\main" -Include *.c,*.h -Recurse

if ($CheckOnly) {
    Write-Host "--> Checking code formatting..." -ForegroundColor Green
    $hasDiff = $false
    foreach ($file in $sourceFiles) {
        & $clangFormat --dry-run --Werror $file.FullName
        if ($LASTEXITCODE -ne 0) {
            Write-Host "[STYLE MISMATCH] $($file.FullName)" -ForegroundColor Red
            $hasDiff = $true
        }
    }
    if ($hasDiff) {
        Write-Host "`n[FAILED] 스타일 규칙과 일치하지 않는 파일이 있습니다." -ForegroundColor Red
        exit 1
    } else {
        Write-Host "`n[SUCCESS] 모든 파일의 코딩 스타일이 일치합니다." -ForegroundColor Green
    }
} else {
    Write-Host "--> Formatting source files..." -ForegroundColor Green
    foreach ($file in $sourceFiles) {
        & $clangFormat -i $file.FullName
        Write-Host " Formatted: $($file.Name)" -ForegroundColor Gray
    }
    Write-Host "`n[SUCCESS] 포맷팅 완료!" -ForegroundColor Green
}
