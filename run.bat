@echo off
REM run.bat - Runs the compiled Snake game

SET EXECUTABLE_NAME=snake_game.exe

IF NOT EXIST %EXECUTABLE_NAME% (
    echo Error: %EXECUTABLE_NAME% not found.
    pause
    goto :eof
)

echo Starting %EXECUTABLE_NAME%...
start "" %EXECUTABLE_NAME%

:eof
