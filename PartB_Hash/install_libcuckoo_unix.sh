#!/bin/bash

# Set the script's directory as the working directory
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Set the GitHub repository URL
REPO_URL=https://github.com/efficient/libcuckoo.git

# Set the subdirectory name
SUBDIR=cuckoohash

# Set the library and installation directories (in subdirectory)
LIBDIR=libcuckoo
INSTALLDIR=install
BUILDDIR=build

# Clone the repository or pull changes if it already exists
git_clone_or_pull(){
	cd "$WORK_DIR/$SUBDIR" || exit 1
	
	if [ -d "$LIBDIR" ]; then
		cd "$LIBDIR"
		git pull origin master
	else
		git clone $REPO_URL "$LIBDIR"
		cd "$LIBDIR"
	fi
	
	if [ $? -ne 0 ]; then
		echo "Error in 'git'."
		exit $?
	fi
}

# Build and install libcuckoo
build_and_install(){
	# create and change to build directory
	mkdir -p "$WORK_DIR/$SUBDIR/$BUILDDIR/$LIBDIR"
	cd "$WORK_DIR/$SUBDIR/$BUILDDIR/$LIBDIR" || exit 1
	
	cmake \
		-B . -S "$WORK_DIR/$SUBDIR/$LIBDIR" \
		-DCMAKE_INSTALL_PREFIX="$WORK_DIR/$SUBDIR/$INSTALLDIR"
    
	
	make all
	if [ $? -ne 0 ]; then
		echo "Error in 'make'."
		exit $?
	fi
		
	make install
	if [ $? -ne 0 ]; then
		echo "Error in 'make'."
		exit $?
	fi
    
	# Delete CMake cache after installation
    rm -f CMakeCache.txt
}

## SCRIPT STARTS HERE ##
# Check if the installation directory exists
if [ -d "$WORK_DIR/$SUBDIR/$INSTALLDIR/include/libcuckoo" ]; then
    echo "libcuckoo is already installed in $WORK_DIR/$SUBDIR/$INSTALLDIR. Skipping installation."
else
    # Create the subdirectories if they don't exist
    mkdir -p "$WORK_DIR/$SUBDIR"
    mkdir -p "$WORK_DIR/$SUBDIR/$INSTALLDIR"
	mkdir -p "$WORK_DIR/$SUBDIR/$BUILDDIR"

    # Change to the subdirectory
    cd "$WORK_DIR/$SUBDIR" || exit 1
	
	# Clone the repository or pull changes
	git_clone_or_pull
	
	# Build and install libcuckoo
	build_and_install
	
    echo "libcuckoo has been successfully cloned and installed."
fi
