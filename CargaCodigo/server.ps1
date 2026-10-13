$ports = 8000..8015
$listener = $null
$boundPort = $null

foreach ($p in $ports) {
    try {
        $tempListener = New-Object System.Net.HttpListener
        $prefix = "http://localhost:$p/"
        $tempListener.Prefixes.Add($prefix)
        $tempListener.Start()
        $listener = $tempListener
        $boundPort = $p
        break
    } catch {
        if ($tempListener) {
            $tempListener.Close()
        }
    }
}

if ($null -eq $listener) {
    Write-Host "ERROR: No se encontró ningún puerto disponible entre 8000 y 8015." -ForegroundColor Red
    exit 1
}

$url = "http://localhost:$boundPort/"
Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "   MediaLab UniOvi - Cargador Web ESP32 (Puerto $boundPort)" -ForegroundColor Cyan
Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "`n[OK] Servidor activo en: $url" -ForegroundColor Green
Write-Host "[OK] Abriendo navegador web automaticamente..." -ForegroundColor Yellow

# Abrir el navegador en el puerto exacto seleccionado
Start-Process $url

Write-Host "`n>>> Por favor, NO cierres esta ventana hasta terminar de flashear el ESP32. <<<" -ForegroundColor White
Write-Host "Para detener el servidor cuando termines, pulsa Ctrl+C o cierra esta ventana.`n"

$mimeTypes = @{
    ".html" = "text/html; charset=utf-8"
    ".htm"  = "text/html; charset=utf-8"
    ".js"   = "application/javascript; charset=utf-8"
    ".json" = "application/json; charset=utf-8"
    ".bin"  = "application/octet-stream"
    ".css"  = "text/css; charset=utf-8"
    ".ico"  = "image/x-icon"
}

$baseDir = $PSScriptRoot

while ($listener.IsListening) {
    try {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response

        $rawUrl = [System.Uri]::UnescapeDataString($request.Url.LocalPath.TrimStart('/'))
        if ([string]::IsNullOrWhiteSpace($rawUrl) -or $rawUrl -eq "") {
            $rawUrl = "index.html"
        }

        $filePath = Join-Path $baseDir $rawUrl

        if (Test-Path $filePath -PathType Leaf) {
            $ext = [System.IO.Path]::GetExtension($filePath).ToLower()
            $contentType = if ($mimeTypes.ContainsKey($ext)) { $mimeTypes[$ext] } else { "application/octet-stream" }
            $response.ContentType = $contentType
            $bytes = [System.IO.File]::ReadAllBytes($filePath)
            $response.ContentLength64 = $bytes.Length
            $response.OutputStream.Write($bytes, 0, $bytes.Length)
        } else {
            $response.StatusCode = 404
            $buffer = [System.Text.Encoding]::UTF8.GetBytes("404 - Archivo no encontrado")
            $response.ContentLength64 = $buffer.Length
            $response.OutputStream.Write($buffer, 0, $buffer.Length)
        }
        $response.Close()
    } catch {
        # Conexión cerrada o interrupción no crítica
    }
}
