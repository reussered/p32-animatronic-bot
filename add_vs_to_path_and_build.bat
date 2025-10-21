@echo off
REM Add Visual Studio Build Tools to PATH and run cl.exe
REM Update this path to match your Visual Studio installation if needed
set "VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64"

REM Add to PATH for this session
set "PATH=%PATH%;%VSPATH%"

REM Compile the test file
cl /EHsc /std:c++17 /Iinclude\core\memory include\core\memory\test_shared_memory.cpp
