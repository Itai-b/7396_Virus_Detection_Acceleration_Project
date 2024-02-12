#!/bin/bash

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Initialize variables
rules_fil="snort3-community.rules"
save_path=$WORK_DIR

# Parse command line arguments
while getopts "r:p:" opt; do
  case $opt in
	r) rules_file="$OPTARG"
	;;
	p) save_path="$OPTARG"
	;;
	\?) echo "Invalid option -$OPTARG" >&2
	exit 1
	;;
	:)
	echo "Option -$OPTARG requires an argument." >&2
	exit 1
	;;
  esac
done

# Set up the python script for running
chmod +x SnortRulesParser.py
dos2unix SnortRulesParser.py > /dev/null 2>&1

# Run the python script
python3 SnortRulesParser.py $rules_file -p $save_path -j -d
