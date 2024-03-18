@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Usage: %0 output_directory
    exit /b 1
)

set "output_directory=%~1"

if exist "%output_directory%" (
    rmdir /s /q "%output_directory%"
)
mkdir "%output_directory%"

set "file_list="

call :process_directory "%CD%\scriptCore"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (MonadoScriptCore.dll)...
csc -target:library -debug -out:"%output_directory%\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of MonadoScriptCore.dll failed.
    goto :end
) else (
    echo Compilation of MonadoScriptCore.dll succeeded. Output: %output_directory%\MonadoScriptCore.dll
)

set "file_list="

call :process_directory "%CD%\editor\sandboxProject\scripts"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (Sandbox.dll)...
csc -target:library -debug -out:"%output_directory%\Sandbox.dll" -r:"%output_directory%\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of Sandbox.dll failed.
    goto :end
) else (
    echo Compilation of Sandbox.dll succeeded. Output: %output_directory%\Sandbox.dll
)

:end
endlocal
goto :eof

:process_directory
for /r %1 %%f in (*.cs) do (
    set "file_list=!file_list! "%%f""
)
goto :eof
