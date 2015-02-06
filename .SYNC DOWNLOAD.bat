@echo off
echo Are you sure you wish to GET the new data from the git repository?
set /p answer=
if %answer% NEQ y exit

git pull
echo.
pause
