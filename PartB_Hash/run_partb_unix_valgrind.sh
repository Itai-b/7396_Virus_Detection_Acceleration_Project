#!/bin/bash

# Should fix clock skews
# sudo ntpdate -s time.nist.gov

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SUBDIR=cuckoohash
INSTALLDIR=install
JSONNAME=parta_data_by_exactmatch.json
COPYJSONFROM=../Data
CPYJSONTO=cuckoohash/src

# Set up the installation scripts for running
chmod +x install_libcuckoo_unix.sh
dos2unix install_libcuckoo_unix.sh
chmod +x install_nlohmann_json_unix.sh
dos2unix install_nlohmann_json_unix.sh

# Fix clock skews (WSL bug)
sudo hwclock -s


# Run the installation scripts:
./install_libcuckoo_unix.sh
./install_nlohmann_json_unix.sh

# Check for potential errors:
if [ ! -d "$WORK_DIR/$SUBDIR" ]; then
	echo "$WORK_DIR/$SUBDIR not found."
	exit 1
elif [ ! -d "$WORK_DIR/$CPYJSONTO" ]; then
	echo "Couldn't find $WORK_DIR/$CPYJSONTO to copy .json file to."
	exit 1
elif [ ! -d "$WORK_DIR/$CPYJSONFROM" ]; then
	echo "Couldn't find $WORK_DIR/$CPYJSONFROM to copy .json file from."
	exit 1
fi

# Copy the updated extracted exact_matches_hex.json from Part A, to the relevant Part B folder
cd "$WORK_DIR"
cp "$WORK_DIR/../Data/$JSONNAME" "$WORK_DIR/$CPYJSONTO/" || { echo "Error: Copying .json file failed."; exit 1; }

# Prepare the build folder
cd "$WORK_DIR/$SUBDIR"
# rm -rf build
mkdir -p build
cd build

# Configure cmake library path, which is shown on CMakePresets.json
cmake -DCMAKE_LIBRARY_PATH="../install" ..

# Build and run the project, taking the path to the .json file as an argument:
make all || exit 1
valgrind src/cuckoohash "$WORK_DIR/$CPYJSONTO/$JSONNAME"