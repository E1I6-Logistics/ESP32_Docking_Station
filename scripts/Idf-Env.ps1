function Import-EspIdfEnv {
    if (Get-Command idf.py -ErrorAction SilentlyContinue) {
        return
    }

    $idfPath = "C:\Espressif\frameworks\esp-idf-v5.3"
    $idfToolsPath = "C:\Espressif"

    if (Test-Path "$idfPath\export.ps1") {
        $env:IDF_PATH = $idfPath
        $env:IDF_TOOLS_PATH = $idfToolsPath
        $env:PATH = "C:\Espressif\tools\idf-git\2.44.0\cmd;C:\Espressif\tools\idf-python\3.11.2;C:\Espressif\tools\idf-python\3.11.2\Scripts;$env:PATH"
        . "$idfPath\export.ps1" | Out-Null
    }
}
