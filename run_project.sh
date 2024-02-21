#!/bin/bash

# Define some colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Check if more than one argument is provided
if [ $# -gt 1 ]; then
    echo "Error: Too many arguments. Only a .rules file is expected."
    exit 1
fi

# Check if the first argument is provided and it ends with .rules
if [[ $1 ]]; then
	if [[$1 == *.rules]]; then
		echo "Processing $1..."
	else
		echo "A wrong .rules file was incorrect! Stopping the script..."
		exit 1
	fi
else
    echo "No .rules file provided... will parse the snort3-community.rules"
fi

# Run PartA - Data Process
if [[$1]]; then
	command PartA_Data_Process/run_parta_unix.sh -r $1 -p ./Data/PartA_Data