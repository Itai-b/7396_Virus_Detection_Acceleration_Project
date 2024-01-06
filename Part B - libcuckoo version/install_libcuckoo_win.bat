@echo off
setlocal

REM Set the script's directory as the working directory
set "WORK_DIR=%~dp0"

REM Set the GitHub repository URL
set "REPO_URL=https://github.com/efficient/libcuckoo.git"

REM Set the subdirectory name
set "SUBDIR=cuckoohash"

REM Set the library and installation directories (in subdirectory)
set "LIBDIR=libcuckoo"
set "INSTALLDIR=install"

REM Create the subdirectory if it doesn't exist
if not exist "%WORK_DIR%\%SUBDIR%\" mkdir "%WORK_DIR%\%SUBDIR%"
if not exist "%WORK_DIR%\%SUBDIR%\%INSTALLDIR%\" mkdir "%WORK_DIR%\%SUBDIR%\%INSTALLDIR%"

REM Change to the subdirectory
cd /d "%WORK_DIR%\%SUBDIR%" || exit /b 1

REM Clone the repository or pull changes if it already exists
if exist "%LIBDIR%" (
    cd "%LIBDIR%"
    git pull origin master
) else (
    git clone %REPO_URL% "%LIBDIR%"
    cd "%LIBDIR%"
)

REM Build and install libcuckoo
cd /d "%WORK_DIR%\%SUBDIR%\%LIBDIR%"
cmake -DCMAKE_INSTALL_PREFIX="%WORK_DIR%\%SUBDIR%\%INSTALLDIR%"
cmake --build . --config Release
cmake --install .

echo "libcuckoo has been successfully cloned and installed."
