@echo off
echo ===========================================
echo Compiling Shop Stock Controller to WASM
echo ===========================================

:: Activate Emscripten silently if run from within emsdk context, or globally if available
call C:\Users\Administrator\emsdk\emsdk_env.bat

:: Execute em++ compiler with advanced Asyncify flags and exported string converters needed for EM_JS input
em++ main.cpp -o index.js -s ASYNCIFY=1 -s "ASYNCIFY_IMPORTS=['getConsoleInputJS']" -s "EXPORTED_FUNCTIONS=['_malloc','_free','_main']"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Compilation completed automatically!
echo Output built successfully into index.js and index.wasm.
echo.
echo Launching local HTTP Server...
python -m http.server 8080
pause
