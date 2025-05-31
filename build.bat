@echo off
REM USAGE: build.bat [compiler]
REM        build.bat cl - default
REM                  gcc
REM
REM Examples:
REM   build.bat      (default, uses cl)
REM   build.bat cl
REM   build.bat gcc

REM --- Configuration ---
SET SRC_FILES=main.c game.c
SET OUTPUT_EXE=snake_game.exe
SET C_STANDARD_GCC=-std=c17
SET C_STANDARD_CL=/std:c17
SET WARNINGS_GCC=-Wall -Wextra
SET WARNINGS_CL=/W4
SET INCLUDE_DIRS=/I.

REM --- Libraries ---
SET LIBS_CL=gdi32.lib user32.lib kernel32.lib
SET LIBS_GCC=-lgdi32 -luser32 -lkernel32

REM --- Compiler selection ---
SET COMPILER_CHOICE=cl
IF /I "%COMPILER_CHOICE%"=="" SET COMPILER_CHOICE=cl
IF /I "%COMPILER_CHOICE%"=="msvc" SET COMPILER_CHOICE=cl

REM --- Clean previous build ---
IF EXIST %OUTPUT_EXE% del %OUTPUT_EXE%
IF EXIST *.obj del *.obj
IF EXIST *.o del *.o

REM --- Compilation and Linking ---
echo Building Snake game with %COMPILER_CHOICE%...

IF /I "%COMPILER_CHOICE%"=="cl" (
    echo Using Microsoft C/C++ Compiler (cl.exe)
    REM Check if cl is available
    cl /? >nul 2>&1
    IF ERRORLEVEL 1 (
        echo Error: cl.exe not found in PATH.
        echo Please run this script from a Developer Command Prompt for Visual Studio.
        goto :eof
    )
    cl %WARNINGS_CL% %C_STANDARD_CL% %INCLUDE_DIRS% %SRC_FILES% /Fe:%OUTPUT_EXE% %LIBS_CL% /nologo
) ELSE IF /I "%COMPILER_CHOICE%"=="gcc" (
    echo Using GNU Compiler Collection (gcc.exe)
    REM Check if gcc is available
    gcc --version >nul 2>&1
    IF ERRORLEVEL 1 (
        echo Error: gcc.exe not found in PATH.
        echo Please ensure MinGW or a similar GCC distribution is installed and in your PATH.
        goto :eof
    )
    gcc %WARNINGS_GCC% %C_STANDARD_GCC% %INCLUDE_DIRS% %SRC_FILES% -o %OUTPUT_EXE% %LIBS_GCC%
) ELSE (
    echo Error: Unsupported compiler "%COMPILER_CHOICE%".
    echo Please use 'cl' or 'gcc'.
    goto :eof
)

REM --- Check Build Status ---
IF EXIST %OUTPUT_EXE% (
    echo.
    echo Build successful: %OUTPUT_EXE%
) ELSE (
    echo.
    echo Build FAILED.
)

:eof
