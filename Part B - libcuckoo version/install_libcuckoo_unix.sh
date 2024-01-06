#!/bin/bash

# Set the script's directory as the working directory
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Set the GitHub repository URL
REPO_URL=https://github.com/efficient/libcuckoo.git

# Set the subdirectory name
SUBDIR=cuckoohash

# Set the library and installation directories (in subdiretory)
LIBDIR=libcuckoo
INSTALLDIR=install

# Create the subdirectory if it doesn't exist
mkdir -p "$WORK_DIR/$SUBDIR"
mkdir -p "$WORK_DIR/$SUBDIR/$INSTALLDIR"

# Change to the subdirectory
cd "$WORK_DIR/$SUBDIR" || exit 1

# Clone the repository or pull changes if it already exists
if [ -d "$LIBDIR" ]; then
    cd "$LIBDIR"
    git pull origin master
else
    git clone $REPO_URL "$LIBDIR"
    cd "$LIBDIR"
fi

# Build and install libcuckoo
cd "$WORK_DIR/$SUBDIR/$LIBDIR"
cmake -DCMAKE_INSTALL_PREFIX="$WORK_DIR/$SUBDIR/$INSTALLDIR"
make all
make install

echo "libcuckoo has been successfully cloned and installed."
