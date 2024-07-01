from matplotlib import pyplot as plt
import os
import argparse
import json

class DataStructureStats:
    def __init__(self):
        self.avg_success_rate = 0.0
        self.avg_fp_rate = 0.0
        self.size = 0.0
        self.data_entries = []

class DataEntryStats:
    def __init__(self):
        self.original_sids = []
        self.sids_hit = []

def getDataStructure(file):
    data_structure = os.path.basename(os.path.dirname(file)).lower()
    if "search_results" in data_structure:
        data_structure = "hash_table" + data_structure.split("search_results")[1]
    else:
        data_structure = "aho_corasick" + file.split("/")[-1].split(".")[0].split("search_results")[1]
    return data_structure        

def find_file_in_directory(directory, filename="partc_results.json"):
    for root, dirs, files in os.walk(directory):
        if filename in files:
            return os.path.join(root, filename)
    return None

def getMemoryStats(data_structure_type, data_structure_stats,data_path):
    # Read the memory stats from the files
    # search for partc_results.json under data_path
    partc_results = find_file_in_directory(data_path)
    if partc_results is None:
        return
    with open(partc_results) as f:
        memory_data = json.load(f)
        if data_structure_type.startswith("hash_table"):
            return
        threshold = data_structure_type.split("_")[-1]
        for entry in memory_data:
            if entry["threshold"] == int(threshold):
                data_structure_stats.size = entry["size_in_theory"]
                break

def calculateStats(data_structure):
    # Calculate the average success rate and average false positive rate
    total_success_rate = 0.0
    total_fp_rate = 0.0
    for data_entry in data_structure.data_entries:
        # if sids in original_sids are in sids_hit, and the number of sids_hit is greater than 0, then it is a success:
        total_success_rate += sum([1 for sid in data_entry.original_sids if sid in data_entry.sids_hit and data_entry.sids_hit[sid] > 0]) / len(data_entry.original_sids)
        # if sids keys in sids_hit are not in original_sids, then it is a false positive:
        total_fp_rate += sum([1 for sid in data_entry.sids_hit.keys() if sid not in data_entry.original_sids]) / len(data_entry.sids_hit)
    data_structure.avg_success_rate = total_success_rate / len(data_structure.data_entries)
    data_structure.avg_fp_rate = total_fp_rate / len(data_structure.data_entries)
    assert(data_structure.avg_success_rate <= 1.0)
    assert(data_structure.avg_fp_rate <= 1.0)

def main():
    parser = argparse.ArgumentParser(description="Generate a summary of the results")
    parser.add_argument("-p", "--path", help="Path to the results directory", required=True)
    args = parser.parse_args()
    
    # Check if the path given is a directory
    if not os.path.isdir(args.path):
        print("The path given is not a directory")
        return 1
    
    # Get the list of files in the directory (include subdirectories) which are .json files and start with "search_results"
    files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(args.path) for f in filenames if os.path.splitext(f)[1] == '.json' and os.path.basename(f).startswith("search_results")]

    search_data = []
    for file in files:
        with open(file) as f:
            # Get the second directory name
            data_structure_type = getDataStructure(file)
            data_structure_stats = DataStructureStats()
            
            data = json.load(f)
            for entry in data:
                data_entry = DataEntryStats()
                data_entry.original_sids = entry["original_sids"]
                data_entry.sids_hit = {sid: hits for sid, hits in entry["sids_hit"]}
                data_structure_stats.data_entries.append(data_entry)
                calculateStats(data_structure_stats)
            
            getMemoryStats(data_structure_type, data_structure_stats, args.path)
            search_data.append((data_structure_type, data_structure_stats))
    
    # print the results
    for data_structure_type, data_structure_stats in search_data:
        print(f"Data structure: {data_structure_type}")
        print(f"Average success rate: {data_structure_stats.avg_success_rate}")
        print(f"Average false positive rate: {data_structure_stats.avg_fp_rate}")
        print(f"Size: {data_structure_stats.size}")
        print()

if __name__ == "__main__":
    #set working directory to script directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    exit_code = main()
    exit(exit_code)