@echo off

:: These can be modified
set ZMENU_BIN=ZMENU.EXE
::set ZMENU_BIN=ZMENU32.EXE
set ZMENU_PATH=C:\ZMENU
set ZMENU_CONF=ZMENU.TXT
set ZMENU_ARGS=
::set ZMENU_ARGS=%ZMENU_ARGS% /noansi
::set ZMENU_ARGS=%ZMENU_ARGS% /col


:: Should not need to modify anything below this line
set ZMENU_BIN_PATH=%ZMENU_PATH%\%ZMENU_BIN%
set ERRORLEVEL_CONTINUE=0
set ERRORLEVEL_EXIT_ERROR=1

:start
:: Clear errorlevel variable
set errorlevel=%ERRORLEVEL_CONTINUE%

:: Check zmenu binary path
if not exist %ZMENU_BIN_PATH% echo ERROR[zmenu.bat]: Path %ZMENU_BIN_PATH% does not exist
if not exist %ZMENU_BIN_PATH% goto end

:: Start zmenu.exe, read menu from file
cd %ZMENU_PATH%
%ZMENU_BIN% /file %ZMENU_CONF% %ZMENU_ARGS%

:: zmenu.exe exit requested or error
if errorlevel %ERRORLEVEL_EXIT_ERROR% goto end

:: Run selected command
call zzz_tmp.bat

:: Error, for example because of invalid path in menu.txt
if %errorlevel%==%ERRORLEVEL_EXIT_ERROR% goto end

:: Loop, start zmenu.exe again after selected command has finished
goto start

:end
:: Remove temporary file
if exist zzz_tmp.bat del zzz_tmp.bat
