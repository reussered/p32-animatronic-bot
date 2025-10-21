@echo off
REM This script sets up the Visual Studio build environment and compiles test_shared_memory.cpp
REM Update the path below to match your Visual Studio installation if needed

REM Try to auto-detect VS 2019/2022 Community Edition
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=1*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
    if defined VSINSTALL (
        call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat"
    )
)

REM Compile the test file
cl /EHsc /std:c++17 /Iinclude\core\memory include\core\memory\test_shared_memory.cpp
