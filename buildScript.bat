@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Usage: %0 output_directory
    exit /b 1
)

set "output_directory=%~1"

if exist "%output_directory%\monadoScriptCore" (
    rmdir /s /q "%output_directory%\monadoScriptCore"
)
mkdir "%output_directory%\monadoScriptCore"

set "file_list="

call :process_directory "%CD%\mondoScriptCore\src"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (MonadoScriptCore.dll)...
csc -target:library -debug -out:"%output_directory%\monadoScriptCore\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of MonadoScriptCore.dll failed.
    goto :end
) else (
    echo Compilation of MonadoScriptCore.dll succeeded. Output: %output_directory%\MonadoScriptCore.dll
)


if exist "%output_directory%\exampleApp" (
    rmdir /s /q "%output_directory%\exampleApp"
)
mkdir "%output_directory%\exampleApp"

set "file_list="

call :process_directory "%CD%\exampleApp\src"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library (Sandbox.dll)...
csc -target:library -debug -out:"%output_directory%\exampleApp\ExampleApp.dll" -r:"%output_directory%\monadoScriptCore\MonadoScriptCore.dll" %file_list%

if errorlevel 1 (
    echo Compilation of ExampleApp.dll failed.
    goto :end
) else (
    echo Compilation of ExampleApp.dll succeeded. Output: %output_directory%\Sandbox.dll
)

:end
endlocal
goto :eof

:process_directory
for /r %1 %%f in (*.cs) do (
    set "file_list=!file_list! "%%f""
)
goto :eof
