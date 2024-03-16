@echo off
setlocal enabledelayedexpansion

set "file_list="

set "output_dir=%CD%\bin"

if not exist "%output_dir%" (
    mkdir "%output_dir%"
)

call :process_directory "%CD%\scriptCore"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (MonadoScriptCore.dll)...
csc -target:library -out:"%output_dir%\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of MonadoScriptCore.dll failed.
    goto :end
) else (
    echo Compilation of MonadoScriptCore.dll succeeded. Output: %output_dir%\MonadoScriptCore.dll
)

set "file_list="

call :process_directory "%CD%\editor\sandboxProject\scripts"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (Sandbox.dll)...
csc -target:library -out:"%output_dir%\Sandbox.dll" -r:"%output_dir%\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of Sandbox.dll failed.
    goto :end
) else (
    echo Compilation of Sandbox.dll succeeded. Output: %output_dir%\Sandbox.dll
)

:end
endlocal
goto :eof

:process_directory
for /r %1 %%f in (*.cs) do (
    set "file_list=!file_list! "%%f""
)
goto :eof
