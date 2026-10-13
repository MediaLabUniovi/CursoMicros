@echo off
chcp 65001 >nul
title Cargador ESP32 - MediaLab UniOvi
cd /d "%~dp0"

echo =======================================================
echo    MediaLab UniOvi - Cargador Web ESP32 (Puerto 8000)
echo =======================================================
echo.
echo Iniciando servidor web local en http://localhost:8000 ...
echo Abriendo el navegador web automaticamente...
echo (Por favor, NO cierres esta ventana hasta terminar la carga)
echo.

:: Abrir navegador tras 1.5 segundos en segundo plano
start "" cmd /c "timeout /t 2 /nobreak >nul & start http://localhost:8000"

:: Ejecutar servidor HTTP nativo con PowerShell (compatible con cualquier version de Windows)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1"

echo.
echo Servidor detenido.
pause
