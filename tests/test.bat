rem Run this in Developer Command Prompt.

cl main.c
if %errorlevel% neq 0 exit /b %errorlevel%
main.exe
