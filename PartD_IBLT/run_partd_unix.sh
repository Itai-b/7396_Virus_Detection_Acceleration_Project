#!/bin/bash

# Define some colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

# Initialize variables
DATA_DIR="$WORK_DIR/../Data"
SAVE_DIR="$WORK_DIR/../Data/PartD_Data"


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
        echo -e "${RED}matplotlib not installed. Exiting.${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}All required packages are installed.${NC}"

# Check if the data directory exists
if [ ! -d "$DATA_DIR" ]; then
    echo -e "${RED}Data directory not found. Exiting.${NC}"
    exit 1
fi

# Check if the save directory exists
if [ ! -d "$SAVE_DIR" ]; then
    mkdir -p $SAVE_DIR
fi

# Set up the python script for running
chmod +x $WORK_DIR/iblt_theoretical_checks.py
chmod +x $WORK_DIR/iblt.py
dos2unix $WORK_DIR/iblt_theoretical_checks.py > /dev/null 2>&1
dos2unix $WORK_DIR/iblt.py > /dev/null 2>&1


echo -e "${BLUE}Running iblt_theoretical_checks.py on ExactMatches.${NC}"
python3 $WORK_DIR/iblt_theoretical_checks.py -p $DATA_DIR/PartA_Data/parta_data_by_exactmatch.json -n "ExactMatches"

echo -e "${BLUE}Running iblt_theoretical_checks.py on Length4_Gap1_Substrings.${NC}"
python3 $WORK_DIR/iblt_theoretical_checks.py -p $DATA_DIR/PartB_Data/Length4_Gap1/L4_G1_substrings.json -n "L4_G1_Substrings"

echo -e "${BLUE}Running iblt_theoretical_checks.py on Length4_Gap2_Substrings.${NC}"
python3 $WORK_DIR/iblt_theoretical_checks.py -p $DATA_DIR/PartB_Data/Length4_Gap2/L4_G2_substrings.json -n "L4_G2_Substrings"

echo -e "${BLUE}Running iblt_theoretical_checks.py on Length8_Gap1_Substrings.${NC}"
python3 $WORK_DIR/iblt_theoretical_checks.py -p $DATA_DIR/PartB_Data/Length8_Gap1/L8_G1_substrings.json -n "L8_G1_Substrings"

echo -e "${BLUE}Running iblt_theoretical_checks.py on Length8_Gap2_Substrings.${NC}"
python3 $WORK_DIR/iblt_theoretical_checks.py -p $DATA_DIR/PartB_Data/Length8_Gap2/L8_G2_substrings.json -n "L8_G2_Substrings"

echo -e "${GREEN}Finished IBLT Theoretical Checks.${NC}"

echo -e "${BLUE}Running ResultsAnalysis.py${NC}"
chmod +x $WORK_DIR/ResultsAnalysis.py
dos2unix $WORK_DIR/ResultsAnalysis.py > /dev/null 2>&1
python3 $WORK_DIR/ResultsAnalysis.py

echo -e "${GREEN}Finished Results Analysis.${NC}"
echo -e "${GREEN}Finished Part D.${NC}"
echo -e "${BLUE}All results are saved in $SAVE_DIR.${NC}"