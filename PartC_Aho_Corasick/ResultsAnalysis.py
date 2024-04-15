import os
import json
import numpy as np
from matplotlib import pyplot as plt
import os

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
desired_size = (1280, 612)  # Desired size in pixels
dpi = 100  # DPI (dots per inch)

def translateTOKB(size):
    size = size / 1024
    return round(size)

def genPlot(data):
    # create a bar plot that uses data['threshold'] in x-axis and data['aho_corasick_size'] in y-axis
    threshold = [str(entry['threshold']) for entry in data]
    aho_corasick_size = [translateTOKB(entry['aho_corasick_size']) for entry in data]
    aho_corasick_capacity = [entry['exact_matches_inserted'] for entry in data]
    run_time = [entry['run_time'] for entry in data]

    width = 0.3
    x = np.arange(len(threshold))

    fig, ax1 = plt.subplots(figsize=(10,6))

    # Plot the aho_corasick_capacity on the left y-axis
    rects1 = ax1.bar(x + width/2, aho_corasick_capacity, width, label='Aho-Corasick Capacity', color=colors[1])
    ax1.set_ylabel('Aho-Corasick Capacity')

    # Create a second y-axis that shares the same x-axis
    ax2 = ax1.twinx()
    # Plot the aho_corasick_size on the right y-axis
    rects2 = ax2.bar(x - width/2, aho_corasick_size, width, label='Aho-Corasick Size', color=colors[0])
    ax2.set_ylabel('Aho-Corasick Size')

    for i in range(len(threshold)):
        ax1.text(x[i]+width/2, aho_corasick_capacity[i], f'{aho_corasick_capacity[i]}', ha='center', va='bottom', fontsize=6)
        ax2.text(x[i]-width/2, aho_corasick_size[i], f'{aho_corasick_size[i]}[KB]', ha='center', va='bottom', fontsize=6)
        plt.text(x[i], -128, f'avg_run_time=\n{str(run_time[i])}[ms]', fontsize=6, color=colors[3], ha='center')

    plt.xlabel('Threshold (String Length)', labelpad=20)
    fig.legend(loc='upper right', bbox_to_anchor=(0.88, 0.93)) 
    plt.title('Aho-Corasick Size and Capacity per String Length Threshold')
    plt.xticks(x, threshold)  # set the x-ticks to be the threshold values
    plt.tight_layout()
    plt.savefig(os.path.join(SAVE_PATH, 'aho_corasick_plot.png'))
    plt.close

DATA_PATH = os.path.join('..', 'Data', 'PartC_Data')
SAVE_PATH = DATA_PATH
FILE_NAME = 'partc_results.json'
def main():
    # Check if the DATA_PATH exists
    if not os.path.exists(DATA_PATH):
        print(f"Error: {DATA_PATH} does not exist.")
        exit(1)
    # Check if the file exists
    file_path = os.path.join(DATA_PATH, FILE_NAME)
    if not os.path.exists(file_path):
        print(f"Error: {file_path} does not exist.")
        exit(1)
    data = []
    with open(file_path, 'r') as file:
        data = json.load(file)
    # get the max threshold
    max_threshold = data[-1]['threshold']
    max_bits = int(np.log2(max_threshold))
    bits = [2**b for b in range(0,max_bits+1)]
    entries = [entry for entry in data if entry['threshold'] in bits]
    
    genPlot(entries)

if __name__ == "__main__":
    # Set the cwd to the script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    main()