@echo off
chcp 65001 >nul
title Cargador ESP32 - MediaLab UniOvi
cd /d "%~dp0"

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1"

if %ERRORLEVEL% neq 0 (
    echo.
    echo Ocurrio un error al iniciar el servidor.
    pause
)
