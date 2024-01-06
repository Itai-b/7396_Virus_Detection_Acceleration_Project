#!/bin/bash

# Set up the libcuckoo installation script for running
chmod +x install_libcuckoo_unix.sh
dos2unix install_libcuckoo_unix.sh

# Run the libcuckoo installation script, which sets up the following:
#	WORK_DIR=$(cd "$(dirname "$0")" && pwd)
#	SUBDIR=cuckoohash
#	LIBDIR=libcuckoo
#	INSTALLDIR=install
./install_libcuckoo_unix.sh

# Prepare the build folder
cd "$WORK_DIR/$SUBDIR"
mkdir build
cd build

# Configure cmake
cmake -DCMAKE_LIBRARY_PATH=../install ..

# Build and run the project
make all
src/cuckoohash