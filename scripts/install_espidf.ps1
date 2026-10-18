# ==============================================================================
# ESP-IDF v5.3.2 All-In-One Automated Installer Script for Windows
# ==============================================================================
param (
    [string]$InstallDir = "C:\Espressif",
    [string]$IdfVersion = "v5.3.2",
    [switch]$ForceDownload
)

$ErrorActionPreference = "Stop"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " [Vibe Embedded] ESP-IDF Automated Environment Setup" -ForegroundColor Cyan
Write-Host " Target Version: $IdfVersion" -ForegroundColor Green
Write-Host " Install Target: $InstallDir" -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. 인스톨러 다운로드 URL 설정
# Espressif 공식 오프라인 인스톨러 (Python, Xtensa GCC, Git, OpenOCD, USB 드라이버 일체 포함)
$InstallerName = "esp-idf-tools-setup-offline-5.3.2.exe"
$DownloadUrl = "https://dl.espressif.com/dl/esp-idf/$InstallerName"
$DownloadDir = "$env:TEMP\espressif_installer"
$InstallerPath = "$DownloadDir\$InstallerName"

if (-not (Test-Path $DownloadDir)) {
    New-Item -ItemType Directory -Path $DownloadDir -Force | Out-Null
}

# 2. 인스톨러 다운로드 (존재하지 않거나 ForceDownload 옵션 시)
if ((Test-Path $InstallerPath) -and (-not $ForceDownload)) {
    Write-Host "`n[1/3] 이미 다운로드된 인스톨러가 존재합니다: $InstallerPath" -ForegroundColor Green
} else {
    Write-Host "`n[1/3] Espressif 공식 오프라인 인스톨러를 다운로드합니다 (~1.5GB)..." -ForegroundColor Green
    Write-Host "URL: $DownloadUrl" -ForegroundColor Gray
    Write-Host "잠시만 기다려 주세요. 다운로드가 진행 중입니다..." -ForegroundColor Gray
    
    try {
        Start-BitsTransfer -Source $DownloadUrl -Destination $InstallerPath -DisplayName "ESP-IDF Installer Download"
    } catch {
        Write-Host "BITS 전송 실패. 웹 요청으로 직접 다운로드합니다..." -ForegroundColor Yellow
        Invoke-WebRequest -Uri $DownloadUrl -OutFile $InstallerPath
    }
}

# 4. 무인 자동 설치(Silent Install) 실행
Write-Host "`n[2/3] ESP-IDF 무인 자동 설치를 시작합니다..." -ForegroundColor Green
Write-Host "설치 위치: $InstallDir" -ForegroundColor Gray
Write-Host "컴파일러, 파이썬 가상환경, USB 드라이버가 자동으로 설정됩니다." -ForegroundColor Gray

# InnoSetup 기반 스위치: /VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP- /DIR="C:\Espressif"
$installArgs = "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP- /DIR=`"$InstallDir`""

$process = Start-Process -FilePath $InstallerPath -ArgumentList $installArgs -Wait -PassThru

if ($process.ExitCode -eq 0) {
    Write-Host "`n[3/3] ESP-IDF 설치가 성공적으로 완료되었습니다!" -ForegroundColor Green
} else {
    Write-Host "`n[WARN] 인스톨러가 코드 $($process.ExitCode)로 종료되었습니다." -ForegroundColor Yellow
}

# 5. 설치 완료 후 환경 확인 및 바로가기 안내
Write-Host "`n==========================================================" -ForegroundColor Cyan
Write-Host " [설치 완료 및 다음 단계 안내]" -ForegroundColor Cyan
Write-Host " 1. 설치 경로: $InstallDir\frameworks\esp-idf-$IdfVersion" -ForegroundColor Gray
Write-Host " 2. 이제 시작 메뉴에서 'ESP-IDF 5.3 PowerShell' 바로가기가 생성됩니다." -ForegroundColor Gray
Write-Host " 3. VS Code를 재시작하면 프로젝트의 빌드/플래시 Tasks가 바로 작동합니다!" -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan
