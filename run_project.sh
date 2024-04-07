#!/bin/bash

# Define some colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SNORT_RULES_FILE=$WORK_DIR/Auxiliary/snort3-community.rules

# Check if more than one argument is provided
if [ $# -gt 1 ]; then
    echo "Error: Too many arguments. Only a .rules file is expected."
    exit 1
fi

# Check if the first argument is provided and it ends with .rules
if [[ $1 ]]; then
	if [[ $1 == *.rules ]]; then
		echo "Processing $1..."
		SNORT_RULES_FILE=$(realpath $1)
	else
		echo "A wrong .rules file was incorrect! Stopping the script..."
		exit 1
	fi
else
    echo "No .rules file provided... parse snort3-community.rules by default"
fi

echo -e "Welcome and thank you for using our script!"
echo -e "All the relevant data will be saved in the Data folder."

# Prepare to run PartA - Data Process
echo -e "${BLUE}Running PartA - Data Process...${NC}"

mkdir -p $WORK_DIR/Data/PartA_Data
chmod +x $WORK_DIR/PartA_Data_Process/run_parta_unix.sh
dos2unix $WORK_DIR/PartA_Data_Process/run_parta_unix.sh

# Run PartA - Data Process
command $WORK_DIR/PartA_Data_Process/run_parta_unix.sh -r $SNORT_RULES_FILE -p $WORK_DIR/Data/PartA_Data 
# check if partA ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}PartA - Data Process ran successfully!${NC}"
else
	echo -e "${RED}PartA - Data Process was not complited.${NC}"
	exit 1
fi

# Check if the user wants to run PartB
echo -e "${BLUE}Do you want to run PartB - Hash Insertion? enter (${GREEN}y${BLUE}/${RED}n${BLUE})${NC}"
read -p "Enter your choice: " answer
if [ $answer = "${answer#[Yy]}" ]; then
	echo -e "${RED}Exiting the script...${NC}"
	exit 0
fi

# Prepare to run PartB - Hash Insertion
echo -e "${BLUE}Running PartB - Hash Insertion...${NC}"

mkdir -p $WORK_DIR/Data/PartB_Data
chmod +x $WORK_DIR/PartB_Hash/run_partb_unix.sh
dos2unix $WORK_DIR/PartB_Hash/run_partb_unix.sh

# Get the number of tests from the user
echo -e "${BLUE}Enter the number of tests you want to run for each case:${NC}"
read -p "Enter your choice (1-100): " num_of_tests
if ! [[ $num_of_tests =~ ^[0-9]+$ ]]; then
	echo -e "${RED}Invalid input. Exiting the script...${NC}"
	exit 1
fi
if [ $num_of_tests -lt 1 ] || [ $num_of_tests -gt 100 ]; then
	echo -e "${RED}Invalid input. Exiting the script...${NC}"
	exit 1
fi

# Run PartB - Hash Insertion
command $WORK_DIR/PartB_Hash/run_partb_unix.sh -n $num_of_tests
# check if partB ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}PartB - Hash Insertion ran successfully!${NC}"
else
	echo -e "${RED}PartB - Hash Insertion was not complited.${NC}"
	exit 1
fi

# TODO - add PartC - Aho-Corasick execution after implementation.