rem Run this in Developer Command Prompt.

cl main.c ../src/coords_grouped_by_strip.c ../src/platform/dos/bitplane_strip.c
if %errorlevel% neq 0 exit /b %errorlevel%
main.exe
