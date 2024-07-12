#!/bin/bash

# Define some colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SNORT_RULES_FILE=$WORK_DIR/Auxiliary/snort3-community.rules
END_TO_END_TEST=$WORK_DIR/Auxiliary/end_to_end_test.json

# Check for arguments
while getopts ":p:t:" opt; do
    case $opt in
        p)
            SNORT_RULES_FILE="$OPTARG"
            ;;
        t)
            END_TO_END_TEST="$OPTARG"
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done

# Check if -p option is provided and it ends with .rules
if [[ $SNORT_RULES_FILE && $SNORT_RULES_FILE != *.rules ]]; then
    echo "Error: Invalid .rules file provided for -p option."
    exit 1
fi

# Check if -t option is provided and it ends with .json
if [[ $END_TO_END_TEST && $END_TO_END_TEST != *.json ]]; then
    echo "Error: Invalid .json file provided for -t option."
    exit 1
fi

# Get the full path of the provided files
SNORT_RULES_FILE=$(realpath "$SNORT_RULES_FILE")
END_TO_END_TEST=$(realpath "$END_TO_END_TEST")

echo -e "${BLUE}Using the following files:${NC}"
echo -e "Snort Rules File: $SNORT_RULES_FILE"
echo -e "End to End Test File: $END_TO_END_TEST"


# Welcome message

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
	echo -e "${RED}PartA - Data Process was not completed.${NC}"
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
command $WORK_DIR/PartB_Hash/run_partb_unix.sh -n $num_of_tests -t $END_TO_END_TEST
# check if partB ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}PartB - Hash Insertion ran successfully!${NC}"
else
	echo -e "${RED}PartB - Hash Insertion was not completed.${NC}"
	exit 1
fi

# Check if the user wants to run PartC
echo -e "${BLUE}Do you want to run PartC - Aho-Corasick Insertion? enter (${GREEN}y${BLUE}/${RED}n${BLUE})${NC}"
read -p "Enter your choice: " answer
if [ $answer = "${answer#[Yy]}" ]; then
	echo -e "${RED}Exiting the script...${NC}"
	exit 0
fi

# Prepare to run PartC - Aho-Corasick Insertion
echo -e "${BLUE}Running PartC - Aho-Corasick Insertion...${NC}"

mkdir -p $WORK_DIR/Data/PartC_Data
chmod +x $WORK_DIR/PartC_Aho_Corasick/run_partc_unix.sh
dos2unix $WORK_DIR/PartC_Aho_Corasick/run_partc_unix.sh

command $WORK_DIR/PartC_Aho_Corasick/run_partc_unix.sh -t $END_TO_END_TEST
# check if PartC ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}PartC - Aho-Corasick Insertion ran successfully!${NC}"
else
	echo -e "${RED}PartC - Aho-Corasick Insertion was not completed.${NC}"
	exit 1
fi

# Run the Summary script

chmod +x $WORK_DIR/ResultsSummary.py
dos2unix $WORK_DIR/ResultsSummary.py

echo -e "${BLUE}Running the Results Summary script...${NC}"
command python3 $WORK_DIR/ResultsSummary.py -p $WORK_DIR/Data
# check if the summary script ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}Results Summary script ran successfully!${NC}"
else
	echo -e "${RED}Results Summary script was not completed.${NC}"
	exit 1
fi

# Check if the user wants to run PartD
echo -e "${BLUE}Do you want to run PartD - IBLT? enter (${GREEN}y${BLUE}/${RED}n${BLUE})${NC}"
echo -e "${BLUE}It is expected to finish in ~15 min${NC}"
read -p "Enter your choice: " answer
if [ $answer = "${answer#[Yy]}" ]; then
	echo -e "${RED}Exiting the script...${NC}"
	exit 0
fi

# Prepare to run PartD - IBLT
echo -e "${BLUE}Running PartD - IBLT...${NC}"

mkdir -p $WORK_DIR/Data/PartD_Data
chmod +x $WORK_DIR/PartD_IBLT/run_partd_unix.sh
dos2unix $WORK_DIR/PartD_IBLT/run_partd_unix.sh

command $WORK_DIR/PartD_IBLT/run_partd_unix.sh
# check if PartD ran successfully
if [ $? -eq 0 ]; then
	echo -e "${GREEN}PartD - IBLT ran successfully!${NC}"
else
	echo -e "${RED}PartB - IBLT was not completed.${NC}"
	exit 1
fi

echo -e "${BLUE}Finish the execution, exiting...${NC}"