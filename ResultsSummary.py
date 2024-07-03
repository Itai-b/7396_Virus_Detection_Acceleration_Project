import matplotlib.pyplot as plt
import numpy as np
import os
import argparse
import json

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']

class DataStructureStats:
    def __init__(self):
        self.avg_success_rate = 0.0
        self.avg_fp_rate = 0.0
        self.data_structure_size = 0.0
        self.iblt_size = 0.0
        self.total_size = 0.0
        self.data_entries = []

class DataEntryStats:
    def __init__(self):
        self.original_sids = []
        self.sids_hit = []

def getDataStructureType(file):
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
                data_structure_stats.data_structure_size = entry["size_in_theory"]
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

def dataCollection(data_path):
    # Get the list of files in the directory (include subdirectories) which are .json files and start with "search_results"
    files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(data_path) for f in filenames if os.path.splitext(f)[1] == '.json' and os.path.basename(f).startswith("search_results")]

    search_data = []
    for file in files:
        with open(file) as f:
            # Get the second directory name
            data_structure_type = getDataStructureType(file)
            data_structure_stats = DataStructureStats()
            
            data = json.load(f)
            for entry in data:
                data_entry = DataEntryStats()
                data_entry.original_sids = entry["original_sids"]
                data_entry.sids_hit = {sid: hits for sid, hits in entry["sids_hit"]}
                if data_structure_type.startswith("hash_table"):
                    data_structure_stats.data_structure_size = entry["size"]
                #TODO - add iblt size
                data_structure_stats.total_size = data_structure_stats.data_structure_size + data_structure_stats.iblt_size
                data_structure_stats.data_entries.append(data_entry)
                calculateStats(data_structure_stats)
            
            getMemoryStats(data_structure_type, data_structure_stats, data_path)
            search_data.append((data_structure_type, data_structure_stats))
    
    # print the results
    for data_structure_type, data_structure_stats in search_data:
        print(f"Data structure: {data_structure_type}")
        print(f"Average success rate: {data_structure_stats.avg_success_rate}")
        print(f"Average false positive rate: {data_structure_stats.avg_fp_rate}")
        print(f"Size: {data_structure_stats.total_size}")
        print()
    
    return search_data

def dataVisualization(search_data, title="Data Structure Statistics", data_path=""):
    
    labels = [stats[0] for stats in search_data]
    avg_success_rates = [stats[1].avg_success_rate for stats in search_data]
    avg_fp_rates = [stats[1].avg_fp_rate for stats in search_data]
    data_structure_sizes = [stats[1].data_structure_size for stats in search_data]
    iblt_sizes = [stats[1].iblt_size for stats in search_data]

    # Set up the figure and axes
    fig, ax1 = plt.subplots(figsize=(12, 8))
    ax1.spines['top'].set_visible(False)
    ax1.spines['right'].set_visible(False)

    # Set the width of each bar and positions
    width = 0.05  # Narrow bars
    space = 0.5  # Reduced space between indices
    x = np.arange(len(labels)) * space

    # Plot bars
    bar1 = ax1.bar(x - width/2, avg_success_rates, width, label='Avg Success Rate', color=colors[0], alpha=0.7)
    bar2 = ax1.bar(x + width/2, avg_fp_rates, width, label='Avg FP Rate', color=colors[1], alpha=0.7)

    # Set labels and title
    ax1.set_ylabel('Rate')
    ax1.set_title(title)

    font_size = 12
    if (title == "Aho-Corasick Statistics"):
        labels = [label.split("aho_corasick_")[1] for label in labels]
        font_size = 10

    # Set x-axis ticks with horizontal labels
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels, rotation=0, ha='center', fontsize=font_size)

    # Add legends
    ax1.legend(loc='upper right')

    # Add data structure size and IBLT size as text


    for i, (ds_size, iblt_size) in enumerate(zip(data_structure_sizes, iblt_sizes)):
        ax1.text(x[i], -0.05, f'DS Size: {ds_size/1024:.0f}[KB]\nIBLT Size: {iblt_size/1024:.0f}[KB]', 
                ha='center', va='top', transform=ax1.get_xaxis_transform(), fontsize=font_size, color='black')

    # Add value labels above each bar
    for rect in bar1:
        height = rect.get_height()
        ax1.text(rect.get_x() + rect.get_width()/2., height,
                f'{(height*100):.2f}%',
                ha='center', va='bottom', fontsize=10, color='black')

    for rect in bar2:
        height = rect.get_height()
        ax1.text(rect.get_x() + rect.get_width()/2., height,
                f'{(height*100):.2f}%',
                ha='center', va='bottom', fontsize=10, color='black')

    # Set y-axis limits to leave space for text above
    ax1.set_ylim(0, max(max(avg_success_rates), max(avg_fp_rates)) * 1.1)

    # Set x-axis limits to focus on the data points
    ax1.set_xlim(x[0] - space/2, x[-1] + space/2)

    # Adjust layout and display the plot
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.25)  # Adjust bottom margin for text
    #plt.show()
    plt.savefig(os.path.join(data_path, title.replace(" ", "_") + ".png"),  bbox_inches='tight', dpi=300)

def bestDataStructures(search_data, category, weights=[0.5, 0.5]):
    if sum(weights) != 1:
        print("The sum of weights must be 1")
        exit(1)
    best_data_structure = None
    best_score = 0
    for data_structure_type, data_structure_stats in search_data:
        if data_structure_type.startswith(category):
            score = data_structure_stats.avg_success_rate * weights[0] + (1 - data_structure_stats.avg_fp_rate) * weights[1]
            if score > best_score:
                best_data_structure = (data_structure_type, data_structure_stats)
                best_score = score
    return best_data_structure

def main():
    
    parser = argparse.ArgumentParser(description="Generate a summary of the results")
    parser.add_argument("-p", "--path", help="Path to the results directory", required=True)
    args = parser.parse_args()
    
    # Check if the path given is a directory
    if not os.path.isdir(args.path):
        print("The path given is not a directory")
        return 1
    
    search_data = dataCollection(args.path)
    hash_tables = [data for data in search_data if data[0].startswith("hash_table")]
    aho_corasicks = [data for data in search_data if data[0].startswith("aho_corasick")]
    dataVisualization(hash_tables, "Hash Table Statistics", args.path)
    dataVisualization(aho_corasicks, "Aho-Corasick Statistics", args.path)
    best_hash_table = bestDataStructures(search_data, "hash_table",[0.75,0.25])
    best_aho_corasick = bestDataStructures(search_data, "aho_corasick",[0.75,0.25])
    dataVisualization([best_hash_table, best_aho_corasick], "Best Data Structures in term of Success Rate and False Positive Rate", args.path)
    
if __name__ == "__main__":
    #set working directory to script directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    exit_code = main()
    exit(exit_code)