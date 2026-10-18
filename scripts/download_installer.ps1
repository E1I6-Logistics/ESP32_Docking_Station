$ProgressPreference = 'SilentlyContinue'
$url = "https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-5.3.exe"
$outDir = "$env:TEMP\espressif_installer"
if (-not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
}
$outPath = "$outDir\esp-idf-tools-setup-offline-5.3.exe"

Write-Host "--> Starting download from: $url"
& curl.exe -L --progress-bar -o $outPath $url

if (Test-Path $outPath) {
    $size = (Get-Item $outPath).Length
    Write-Host "DOWNLOAD_SUCCESS: $outPath ($size bytes)"
} else {
    Write-Host "DOWNLOAD_FAILED"
}
