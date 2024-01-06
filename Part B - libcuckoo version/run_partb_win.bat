@echo off
setlocal

REM Set up the libcuckoo installation script for running
chmod +x install_libcuckoo_win.bat
dos2unix install_libcuckoo_win.bat

REM Run the libcuckoo installation script
.\install_libcuckoo_win.bat

REM Set up variables
	REM set "WORK_DIR=%~dp0"
	REM set "SUBDIR=cuckoohash"
	REM set "LIBDIR=libcuckoo"
	REM set "INSTALLDIR=install"

REM Prepare the build folder
cd /d "%WORK_DIR%\%SUBDIR%" || exit /b 1
mkdir build
cd build

REM Configure cmake
cmake -DCMAKE_LIBRARY_PATH=..\install ..

REM Build and run the project
cmake --build . --config Release
src\cuckoohash
