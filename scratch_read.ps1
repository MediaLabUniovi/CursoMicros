$port = New-Object System.IO.Ports.SerialPort "COM5", 115200, "None", 8, "One"
$port.ReadTimeout = 1000

try {
    $port.Open()
    Write-Host "Reseteando ESP32 mediante DTR/RTS..."
    # Reset ESP32: DTR high, RTS low, then toggle
    $port.DtrEnable = $false
    $port.RtsEnable = $true
    Start-Sleep -Milliseconds 100
    $port.RtsEnable = $false
    Start-Sleep -Milliseconds 100

    Write-Host "Leyendo salida de arranque del ESP32..."
    $endTime = (Get-Date).AddSeconds(5)
    $linesRead = 0
    while ((Get-Date) -lt $endTime) {
        try {
            $line = $port.ReadLine()
            Write-Host "ESP32 -> $line"
            $linesRead++
        } catch [System.TimeoutException] {
            # timeout
        }
    }
    if ($linesRead -eq 0) {
        Write-Host "No se han recibido lineas tras el reset."
    }
} catch {
    Write-Host "Error: $_"
} finally {
    if ($port.IsOpen) {
        $port.Close()
        Write-Host "Puerto cerrado."
    }
}
