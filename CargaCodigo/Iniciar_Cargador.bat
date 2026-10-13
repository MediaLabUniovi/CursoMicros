@echo off
chcp 65001 >nul
title Cargador ESP32 - MediaLab UniOvi
cd /d "%~dp0"

where node >nul 2>nul
if %ERRORLEVEL% equ 0 (
    node "%~dp0servidor.js"
    goto end
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1"

:end
if %ERRORLEVEL% neq 0 (
    echo.
    echo Ocurrio un error al iniciar el servidor local.
    pause
)
