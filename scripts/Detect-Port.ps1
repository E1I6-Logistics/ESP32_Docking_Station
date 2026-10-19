param()

$ports = [System.IO.Ports.SerialPort]::GetPortNames()

if ($ports -and $ports.Count -gt 0) {
    Write-Output $ports[0]
}
