@echo off
setlocal

REM Resolve project root (TestSrcProject)
set SCRIPT_DIR=%~dp0
for %%I in ("%SCRIPT_DIR%..") do set PROJECT_ROOT=%%~fI

REM Resolve repo root (parent of TestSrcProject)
for %%I in ("%PROJECT_ROOT%\..") do set REPO_ROOT=%%~fI

set MIRROR_EXE=%REPO_ROOT%\bin\Release\clang-mirror.exe
set SOURCE_LIST=%PROJECT_ROOT%\registration_srcs.txt
set INCLUDE_DIR=%PROJECT_ROOT%\inc
set OUT_DIR=%PROJECT_ROOT%

if not exist "%MIRROR_EXE%" (
echo ERROR: clang-mirror not found.
echo Expected at:
echo %MIRROR_EXE%
exit /b 1
)

if not exist "%SOURCE_LIST%" (
echo ERROR: registration_srcs.txt not found.
echo Expected at:
echo %SOURCE_LIST%
exit /b 1
)

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

set FILE_ARGS=

for /f "usebackq delims=" %%F in ("%SOURCE_LIST%") do (
call :add_file "%%F"
)

echo Running:
echo "%MIRROR_EXE%" %FILE_ARGS% -out-dir="%OUT_DIR%" -- -I"%INCLUDE_DIR%" -std=c++20
echo.

"%MIRROR_EXE%" %FILE_ARGS% -out-dir="%OUT_DIR%" -- -I"%INCLUDE_DIR%" -std=c++20

if errorlevel 1 (
echo clang-mirror failed.
exit /b 1
)

echo Registration code generation process finished.

endlocal
exit /b 0

:add_file
set LINE=%~1

if "%LINE%"=="" goto :eof
if "%LINE:~0,1%"=="#" goto :eof

set FILE_ARGS=%FILE_ARGS% "%PROJECT_ROOT%\%LINE%"
goto :eof