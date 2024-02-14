#!/bin/bash

# Define some colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Initialize variables
rules_fil="snort3-community.rules"
save_path=$WORK_DIR


# Check if Python is installed
if ! command -v python3 &>/dev/null; then
    echo "python is not installed. Do you want to install it? (y/n)"
	read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
        sudo apt install python3
    else
        echo -e "${RED}Stopping script.${NC}"
        exit 1
    fi
fi

# Check if matplotlib is installed
if ! python3 -c "import matplotlib" &> /dev/null; then
    echo "matplotlib is not installed. Do you want to install it? (y/n)"
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
		sudo apt install python3-matplotlib
	else
        echo -e "{$RED}matplotlib not installed. Exiting.${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}All required packages are installed.${NC}"

# Parse command line arguments
while getopts "r:p:" opt; do
  case $opt in
	r) rules_file="$OPTARG"
	;;
	p) save_path="$OPTARG"
	;;
	\?) echo -e "${RED}Invalid option -$OPTARG${NC}" >&2
	exit 1
	;;
	:)
	echo "${RED}Option -$OPTARG requires an argument.${NC}" >&2
	exit 1
	;;
  esac
done

# Set up the python script for running
chmod +x SnortRulesParser.py
dos2unix SnortRulesParser.py > /dev/null 2>&1

echo "Executing SnortRulesParser.py."
# Run the python script
python3 SnortRulesParser.py $rules_file -p $save_path -j -d
