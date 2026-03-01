@echo off
::
:: build_sim.bat – Build the Raspberry Pi 4 AUTOSAR simulator for Windows
::
:: Requirements:
::   MinGW-w64 (gcc.exe must be on PATH)
::   Download: https://www.mingw-w64.org/  or via MSYS2: pacman -S mingw-w64-x86_64-gcc
::
:: Usage:
::   cd simulator
::   build_sim.bat          :: build
::   build_sim.bat run      :: build and run (10-second simulation)
::   build_sim.bat clean    :: remove build artefacts
::

setlocal
set SCRIPT_DIR=%~dp0
set APP_DIR=%SCRIPT_DIR%..\app
set OUTPUT=%SCRIPT_DIR%sim_app.exe

:: ---- Clean ----
if /I "%1"=="clean" (
    if exist "%OUTPUT%" del /F /Q "%OUTPUT%"
    echo Cleaned.
    goto :eof
)

:: ---- Check GCC ----
where gcc >nul 2>&1
if errorlevel 1 (
    echo Error: gcc not found.
    echo Install MinGW-w64 and add its bin directory to PATH.
    echo   MSYS2: pacman -S mingw-w64-x86_64-gcc
    echo   Download: https://www.mingw-w64.org/
    exit /b 1
)

echo ================================================
echo   Trampoline AUTOSAR OS - PC Simulator Build
echo ================================================
echo.

:: ---- Compile ----
::   SIMULATOR        – marks a simulator build
::   SIM_DURATION_SEC – run for 10 s then exit (remove to run indefinitely)
gcc -O2 -Wall -Wextra ^
    -DSIMULATOR ^
    -DSIM_DURATION_SEC=10 ^
    -I"%APP_DIR%" ^
    "%APP_DIR%\main.c" ^
    "%SCRIPT_DIR%sim_os.c" ^
    "%SCRIPT_DIR%sim_drivers.c" ^
    -o "%OUTPUT%"

if errorlevel 1 (
    echo Build FAILED.
    exit /b 1
)

echo Build successful: %OUTPUT%
echo.

:: ---- Run ----
if /I "%1"=="run" (
    echo Running simulator (10 seconds^)...
    echo ---
    "%OUTPUT%"
)
