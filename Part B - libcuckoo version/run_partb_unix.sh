#!/bin/bash

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SUBDIR=cuckoohash
INSTALLDIR=install
JSONNAME = exact_matches_hex.json
COPYJSONFROM = ../Data
CPYJSONTO = cuckoohash/src

# Set up the installation scripts for running
chmod +x install_libcuckoo_unix.sh
dos2unix install_libcuckoo_unix.sh
chmod +x install_nlohmann_json_unix.sh
dos2unix install_nlohmann_json_unix.sh


# Run the installation scripts:
./install_libcuckoo_unix.sh
./install_nlohmann_json_unix.sh

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

# Configure cmake library path, which is shown on CMakePresets.json
cmake -DCMAKE_LIBRARY_PATH=../install ..

# Build and run the project
make all
src/cuckoohash