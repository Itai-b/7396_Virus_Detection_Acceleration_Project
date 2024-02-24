#!/bin/bash

# Should fix clock skews
# sudo ntpdate -s time.nist.gov

# Set up working dir params
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
SUBDIR=cuckoohash
INSTALLDIR=$WORK_DIR/install
JSONPATH=$WORK_DIR/../Data/PartA_Data/parta_data_by_exactmatch.json
DESTPATH=$WORK_DIR/../Data/PartB_Data

# Set up the installation scripts for running
chmod +x $WORK_DIR/install_libcuckoo_unix.sh
dos2unix $WORK_DIR/install_libcuckoo_unix.sh
chmod +x $WORK_DIR/install_nlohmann_json_unix.sh
dos2unix $WORK_DIR/install_nlohmann_json_unix.sh

# Fix clock skews (WSL bug)
sudo hwclock -s


# Run the installation scripts:
$WORK_DIR/install_libcuckoo_unix.sh
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
while getopts "n:" opt; do
  case ${opt} in
    j )
      file_path=$OPTARG
      ;;
    d )
      dest_path=$OPTARG
      ;;
    n )
      num_of_tests=$OPTARG
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

# Now you can use the variables $file_path, $dest_path, and $num_of_tests in your script

# Configure cmake library path, which is shown on CMakePresets.json
cmake -DCMAKE_LIBRARY_PATH="../install" ..

# Build and run the project, taking the path to the .json file as an argument:
make all || exit 1

if [ "$num_of_tests" ]; then
	src/cuckoohash -f "$JSONPATH" -d $DESTPATH -n $num_of_tests || exit 1
else
	src/cuckoohash -f "$JSONPATH" -d $DESTPATH || exit 1
fi

# Run ResultsAnalysis.py
cd $WORK_DIR
chmod +x $WORK_DIR/ResultsAnalysis.py
dos2unix $WORK_DIR/ResultsAnalysis.py
python3 $WORK_DIR/ResultsAnalysis.py || exit 1
