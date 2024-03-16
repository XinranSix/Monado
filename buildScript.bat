@echo off
setlocal enabledelayedexpansion

set "file_list="

call :process_directory "%CD%\scriptCore"

echo File list: %file_list%

echo Compiling all C# files into a dynamic library...
mcs -target:library -out:.\bin\Main.dll %file_list%

if errorlevel 1 (
    echo Compilation failed.
) else (
    echo Compilation succeeded. Output: Main.dll
)

endlocal
goto :eof

:process_directory
for /r %1 %%f in (*.cs) do (
    set "file_list=!file_list! "%%f""
)
goto :eof
