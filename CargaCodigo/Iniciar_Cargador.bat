@echo off
chcp 65001 >nul
title Cargador ESP32 - MediaLab UniOvi
cd /d "%~dp0"

echo =======================================================
echo    MediaLab UniOvi - Cargador Web ESP32 (Puerto 8000)
echo =======================================================
echo.
echo Iniciando servidor local para Web Serial API...
echo Abriendo navegador en http://localhost:8000 ...
echo (Por favor, NO cierres esta ventana mientras cargas el firmware)
echo.

start "" "http://localhost:8000"

where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Usando interprete Python (py)...
    py -3 -m http.server 8000
    goto end
)

where python >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Usando interprete Python (python)...
    python -m http.server 8000
    goto end
)

where node >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Usando Node.js...
    node -e "const http=require('http'),fs=require('fs'),path=require('path');http.createServer((req,res)=>{let f=path.join('.',decodeURIComponent(req.url.split('?')[0]));if(f==='.'||f==='./'||f==='.\\')f='./index.html';if(fs.existsSync(f)&&fs.statSync(f).isFile()){const m={'.html':'text/html; charset=utf-8','.js':'application/javascript; charset=utf-8','.json':'application/json','.bin':'application/octet-stream'}[path.extname(f)]||'application/octet-stream';res.writeHead(200,{'Content-Type':m});fs.createReadStream(f).pipe(res);}else{res.writeHead(404);res.end('Not found');}}).listen(8000,()=>console.log('Servidor en http://localhost:8000'));"
    goto end
)

echo [OK] Usando PowerShell integrado...
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1"

:end
echo Servidor detenido.
pause
