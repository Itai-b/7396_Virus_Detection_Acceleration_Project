#!/bin/bash

# Set up the libcuckoo installation script for running
chmod +x install_libcuckoo_unix.sh
dos2unix install_libcuckoo_unix.sh

# Run the libcuckoo installation script, which sets up the following:
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SUBDIR=cuckoohash
LIBDIR=libcuckoo
INSTALLDIR=install
JSONNAME = exact_matches_hex.json
COPYJSONFROM = ../Data
CPYJSONTO = cuckoohash/src
./install_libcuckoo_unix.sh

if [ -d "$WORK_DIR/$SUBDIR" ]; then
	echo "$WORK_DIR/$SUBDIR not found."
	exit 1
else if [ -d "$WORK_DIR/$CPYJSONTO" ]; then
	echo "Couldn't find $WORK_DIR/$CPYJSONTO to copy .json file to."
	exit 1
else if [ -d "$WORK_DIR/$CPYJSONFROM" ]; then
	echo "Couldn't find $WORK_DIR/$CPYJSONFROM to copy .json file from."
	exit 1
fi

# Copy the updated extracted exact_matches_hex.json
cd "$WORK_DIR"
cp "$WORK_DIR/$CPYJSONFROM/JSONNAME" "$WORK_DIR/$CPYJSONTO/" || { echo "Error: Copying .json file failed."; exit 1; }

# Prepare the build folder
cd "$WORK_DIR/$SUBDIR"
rm -rf build
mkdir build
cd build

# Configure cmake
cmake -DCMAKE_LIBRARY_PATH=../install ..

# Build and run the project
make all
src/cuckoohash