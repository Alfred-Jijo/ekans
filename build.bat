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
