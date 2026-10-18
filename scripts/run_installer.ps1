param (
    [string]$InstallDir = "C:\Espressif"
)

$InstallerPath = "$env:TEMP\espressif_installer\esp-idf-tools-setup-offline-5.3.exe"

if (-not (Test-Path $InstallerPath)) {
    Write-Host "[ERROR] 인스톨러 파일이 없습니다: $InstallerPath" -ForegroundColor Red
    exit 1
}

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " [Vibe Embedded] ESP-IDF v5.3 설치를 시작합니다..." -ForegroundColor Cyan
Write-Host " 설치 경로: $InstallDir" -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan

# Windows UAC(관리자 권한) 요청과 함께 인스톨러 실행
# 사용자 안내를 위해 표준 대화상자와 함께 실행합니다.
Start-Process -FilePath $InstallerPath -Verb RunAs
