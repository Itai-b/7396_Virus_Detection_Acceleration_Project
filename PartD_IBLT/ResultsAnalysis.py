import os
import json
import numpy as np
from matplotlib import pyplot as plt
import os

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
desired_size = (1280, 612)  # Desired size in pixels
dpi = 100  # DPI (dots per inch)

# Calculate figure size in inches
figsize = [i/dpi for i in desired_size]

fig = plt.figure(figsize=figsize, dpi=dpi)

def check_json_file_type(file_path):
    with open(file_path, 'r') as file:
        # Skip leading whitespace
        while True:
            char = file.read(1)
            if not char.isspace():
                break

        if char == '[':
            return 'Array of objects'
        elif char == '{':
            return 'Series of objects'
        else:
            return 'Invalid JSON'

def parse_json(json_path):
    # Read the .json file and parse it to a list of rules
    # Check if .json file is an array of objects or a series of objects
    file_type = check_json_file_type(json_path)
    
    if file_type == 'Invalid JSON':
        print("Error: Invalid JSON file.")
        exit(1)
    rules = []
    with open(json_path, 'r') as file:
        if file_type == 'Series of objects':    
            for line in file:
                data = json.loads(line)
                rules.append(data['rules'])
        else: # Array of objects
            data = json.load(file)
            for obj in data:
                rules.append(obj['rules'])
    return rules

def genIBLTPlot(iblt_paths):
    fig = plt.figure(figsize=figsize, dpi=dpi)
    # Generate a bar chart for iblt listEntries success rate by m
    plot_data = {}
    for iblt_path in iblt_paths:
        with open(iblt_path, 'r') as file:
            filename = iblt_path.split('\\')[-1]
            modified_filename = '_'.join(filename.split('_')[3:]).split('.')[0]
            modified_filename = modified_filename.lower()
            if modified_filename.endswith('exactmatches'):
                modified_filename = 'aho_corasick'
            elif modified_filename.endswith('substrings'):
                # swap the word 'substrings' with 'Bloom Filter'
                modified_filename = modified_filename.replace('substrings', 'hash_table')

            data = []
            for line in file:
                file_data = json.loads(line)
                data.append((file_data['m'], file_data['success_rate']))
            plot_data[modified_filename] = data

    barwidth = 0.1

    # Plot a bar chart by m in x axis and success rate in y axis
    for i, (key, value) in enumerate(plot_data.items()):
        plt.bar([x + barwidth * i for x in range(len(value))], [x[1] for x in value], width=barwidth, label=key)
    plt.xlabel('Number of Cells in IBLT (m)')
    plt.ylabel('Success Rate')
    # change the y-axis to percentage
    plt.axhline(y=1, color='r', linestyle='--', alpha=0.5, linewidth=0.5)
    plt.ylim(0.5, 1.001)
    ax = plt.gca()
    ticks = ax.get_yticks()
    ax.set_yticks(ticks)
    ax.set_yticklabels(['{:.0f}%'.format(x*100) for x in ticks[:-1]] + [''])
    # draw a horizontal red line at y=100%
    plt.title('IBLT ListEntries Mean Success Rate by Number of Cells in IBLT (m)')
    plt.xticks([r + barwidth * 2 for r in range(len(plot_data['aho_corasick']))], [x[0] for x in plot_data['aho_corasick']])

    plt.legend(bbox_to_anchor=(1.2, 1), loc='upper right')
    plt.subplots_adjust(right=0.85)
    plt.savefig(os.path.join(SAVE_PATH, 'iblt_listentries_success_rate.png'))
    plt.close()

def genHeatmap():
    fig = plt.figure(figsize=(10,6))
    # Get the file path
    paths = []
    exactmatch_path = os.path.join(DATA_PATH, 'PartA_Data', 'parta_data_by_exactmatch.json')
    if not os.path.exists(exactmatch_path):
        print(f"File not found: {exactmatch_path}")
        return 1
    paths.append(exactmatch_path)
    partb_data_path = os.path.join(DATA_PATH, 'PartB_Data')
    if not os.path.exists(partb_data_path):
        print(f"Directory not found: {partb_data_path}")
        return 1
    for root, dirs, files in os.walk(partb_data_path):
        for file in files:
            if file.endswith('substrings.json'):
                paths.append(os.path.join(root, file))
    if paths is []:
        print("No substrings.json files found in PartB_Data/")
        return 1
    
    fig, axs = plt.subplots(len(paths), 1)
    
    heatmap_data = []
    largest_rules_len = 0
    for i, path in enumerate(paths):
        name = path.split('\\')[-1].split('.')[0]
        if name.endswith('exactmatch'):
            name = 'exactmatches'
        else:
            name = name.lower()
        rules = parse_json(path)
        rules_lengths = [len(x) for x in rules]
        heatmap_data.append({'name': name, 'rules_lengths': rules_lengths})
        file_largest_rules_len = max(rules_lengths)
        if file_largest_rules_len > largest_rules_len:
            largest_rules_len = file_largest_rules_len

    for i, data in enumerate(heatmap_data):    
        # Create a 2D array where each row is the rules_lengths array
        heatmap_data = np.array(data['rules_lengths'])[np.newaxis,:]
        length_mean = np.mean(heatmap_data).round(2)
        length_median = np.median(heatmap_data).round(2)
        axs[i].imshow(heatmap_data, cmap='cividis', aspect='auto', interpolation='nearest', vmin=0, vmax=largest_rules_len)
        axs[i].set_yticks([])
        axs[i].title.set_fontsize(10)
        axs[i].title.set_position([0.5, 1.5])
        axs[i].title.set_text(f"{data['name']}")
        # Add the mean and median values to the plot below the title

        axs[i].text(0.9, 1.3, f"Mean: {length_mean:.2f}, Median: {length_median:.2f}", \
                    transform=axs[i].transAxes, ha='center', va='top', fontsize=6)
    
    # Add a colorbar on the right side of the plot vertically
    cbar_ax = fig.add_axes([0.9, 0.15, 0.03, 0.7])    
    cbar = fig.colorbar(axs[0].imshow(heatmap_data, cmap='cividis', aspect='auto', interpolation='nearest'), cax=cbar_ax, orientation='vertical')
    plt.setp(cbar.ax.get_yticklabels(), fontsize=6)
    plt.subplots_adjust(hspace=1.2, wspace=0.8, right=0.85)
    plt.suptitle('Number of Rules in Each Exact Match/Substring')
    plt.savefig(os.path.join(SAVE_PATH, 'rules_length_heatmap.png'))
    plt.close()
    
DATA_PATH = os.path.join('..', 'Data')
SAVE_PATH = os.path.join(DATA_PATH, 'PartD_Data')
def main():
    if not os.path.exists(DATA_PATH):
        print(f"Data path not found: {DATA_PATH}")
        return 1
    
    genHeatmap()

    # Get all the .json relevant for the iblt.
    iblt_paths = []
    partd_data_path = os.path.join(DATA_PATH, 'PartD_Data')
    if not os.path.exists(partd_data_path):
        print(f"Directory not found: {partd_data_path}")
        return 1
    for root, dirs, files in os.walk(partd_data_path):
        for file in files:
            if file.startswith('iblt_theoretical_checks') and file.endswith('.json'):
                iblt_paths.append(os.path.join(root, file))

    genIBLTPlot(iblt_paths)

if __name__ == "__main__":
    # Set the cwd to the script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    main()