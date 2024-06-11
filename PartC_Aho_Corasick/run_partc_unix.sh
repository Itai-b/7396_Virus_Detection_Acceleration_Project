#!/bin/bash

# Should fix clock skews
# sudo ntpdate -s time.nist.gov

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SUBDIR=aho_corasick
INSTALLDIR=$WORK_DIR/install
JSONPATH=$WORK_DIR/../Data/PartA_Data/parta_data_by_exactmatch.json
DESTPATH=$WORK_DIR/../Data/PartC_Data

# Set up the installation scripts for running
chmod +x $WORK_DIR/install_nlohmann_json_unix.sh
dos2unix $WORK_DIR/install_nlohmann_json_unix.sh

# Fix clock skews (WSL bug)
sudo hwclock -s

# Run the installation scripts:
$WORK_DIR/install_nlohmann_json_unix.sh

# Check for potential errors:
if [ ! -d "$WORK_DIR/$SUBDIR" ]; then
	echo "$WORK_DIR/$SUBDIR not found."
	exit 1
fi

# rm -rf build
mkdir -p $WORK_DIR/$SUBDIR/build
cd $WORK_DIR/$SUBDIR/build

# Recieve arguments from the user
while getopts "j:d:" opt; do
  case ${opt} in
    j )
      JSONPATH=$OPTARG
      ;;
    d )
      DESTPATH=$OPTARG
      ;;
    \? )
      echo "Invalid option: -$OPTARG" 1>&2
      exit 1
      ;;
    : )
      echo "Option -$OPTARG requires an argument" 1>&2
      exit 1
      ;;
  esac
done

# Configure cmake library path, which is shown on CMakePresets.json
cmake -DCMAKE_LIBRARY_PATH="../install" ..

# Build and run the project, taking the path to the .json file as an argument:
make all || exit 1

# Prepare Input Data and Output Directory
mkdir -p "$DESTPATH"
src/aho_corasick -f "$JSONPATH" -d "$DESTPATH" || exit 1


# Run ResultsAnalysis.py
echo -e "${BLUE}Running ResultsAnalysis.py${NC}"
chmod +x $WORK_DIR/ResultsAnalysis.py
dos2unix $WORK_DIR/ResultsAnalysis.py > /dev/null 2>&1
python3 $WORK_DIR/ResultsAnalysis.py
