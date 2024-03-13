#!/bin/bash

# Check ocrrect call for the script with <folder_name> argument
if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <path_to_zipped_folder_name.tar.gz>"
	exit 1
fi

RULES_DIR="$(dirname "$1")"
UNZIPPED_FOLDER_NAME="$(dirname "$1")/$(basename "$1" .tar.gz)" 

# Unzip folder in the same directory
mkdir -p "$UNZIPPED_FOLDER_NAME"
tar -xzvf "$1" -C "$UNZIPPED_FOLDER_NAME"

# Create or empty the all.rules file
> all.rules

# Find files ending with '.rules' recursively and append them to all.rules file
find "$UNZIPPED_FOLDER_NAME" -name '*.rules' -type f -exec cat {} + >> all.rules

echo "All .rules files were appended to all.rules"