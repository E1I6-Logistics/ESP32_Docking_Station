# ==============================================================================
# Helper to automatically load ESP-IDF Environment in PowerShell
# ==============================================================================
function Import-EspIdfEnv {
    if (Get-Command idf.py -ErrorAction SilentlyContinue) {
        return
    }

    $idfPath = "C:\Espressif\frameworks\esp-idf-v5.3"
    $idfToolsPath = "C:\Espressif"

    if (Test-Path "$idfPath\export.ps1") {
        $env:IDF_PATH = $idfPath
        $env:IDF_TOOLS_PATH = $idfToolsPath
        # 시스템 기본 Python 대신 Espressif 내장 Python/Git 우선 등록
        $env:PATH = "C:\Espressif\tools\idf-git\2.44.0\cmd;C:\Espressif\tools\idf-python\3.11.2;C:\Espressif\tools\idf-python\3.11.2\Scripts;$env:PATH"
        . "$idfPath\export.ps1" | Out-Null
    }
}

Export-ModuleMember -Function Import-EspIdfEnv -ErrorAction SilentlyContinue
