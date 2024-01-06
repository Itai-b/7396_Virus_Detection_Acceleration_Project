@echo off
setlocal

rem Set the working directory to the current directory
set WORK_DIR=%~dp0

rem Set the GitHub repository URL
set REPO_URL=https://github.com/efficient/libcuckoo.git

rem Clone the repository
git clone %REPO_URL% %WORK_DIR%

rem Change to the cloned directory
cd %WORK_DIR%

rem Build and install libcuckoo
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install .

echo "libcuckoo has been successfully cloned and installed."

endlocal