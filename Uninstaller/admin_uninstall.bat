@echo off
reg delete "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v "STM32CubeMX_PATH" /f
set "params=%*"
cd /d "%~dp0" && ( if exist "%temp%\getadmin.vbs" del "%temp%\getadmin.vbs" ) && fsutil dirty query %systemdrive% 1>nul 2>nul || (  echo Set UAC = CreateObject^("Shell.Application"^) : UAC.ShellExecute "cmd.exe", "/c cd ""%~sdp0"" && %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs" && "%temp%\getadmin.vbs" && exit /B )

REM Copy jre to tmp folder
SET src_dir=%~dp0
set tmp_dir=%temp%\MX-JRE-%random%%random%
%windir%\System32\xcopy /y /i /s "%src_dir%/../jre" "%tmp_dir%"
cd %homepath%
REM Launch uninstaller 
IF EXIST "%tmp_dir%\bin\javaw.exe" START /WAIT "" "%tmp_dir%\bin\javaw.exe" "-jar" "%src_dir%\uninstaller.jar"