@echo off
echo Are you sure you wish to SEND the new data to the git repository?
set /p answer=
if %answer% NEQ y exit

git add -A
echo Enter what changes you've made:
set /p changes=
git commit -m "%changes%"
git push
echo.
pause