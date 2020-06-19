@echo off

:start
:: Clear errorlevel variable (errorlevel and %errorlevel% seem to be different variables in DOSBox)
set errorlevel=0

:: Check zmenu.exe path
if not exist c:\zmenu\zmenu.exe echo ERROR[zmenu.bat]: Default path c:\zmenu\zmenu.exe does not exist
if not exist c:\zmenu\zmenu.exe goto end

:: Start zmenu.exe, read menu from zmenu.txt
cd c:\zmenu
zmenu.exe /file zmenu.txt

:: zmenu.exe exit requested or error
if errorlevel 1 goto end

:: Run selected command
call zzz_tmp.bat

:: Error, for example because of invalid path in menu.txt
if %errorlevel%==1 goto end

:: Loop, start zmenu.exe again after selected command has finished
goto start

:end
:: Remove temporary file
if exist zzz_tmp.bat del zzz_tmp.bat
