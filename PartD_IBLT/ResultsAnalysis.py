import os
import json
from matplotlib import pyplot as plt
import os

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
plt.figure(figsize=(10, 6))

def genPlots(file_path):
    type = file_path.split('/')[-1].split('_')[-1].split('.')[0]
    rules_set = []
    with open(file_path, 'r') as file:
        for line in file:
            data = json.loads(line)
            exact_match = data['exact_match']
            if len(exact_match) <= THRESHOLD:
                continue
            rules_set.append(data['rules'])
    print(f"Number of sets: {len(rules_set)}")
    # Create a bar chart for rules length in each set 
    rules_lengths = [len(rules) for rules in rules_set]

    # Create a bar chart for rules length by count
    plt.bar(range(len(rules_lengths)), rules_lengths)
    plt.xlabel('Set')
    plt.ylabel('Rules Length')
    plt.title(f'Number of Rules under {type}')

    plt.savefig(os.path.join(SAVE_PATH, f'rules_in_{type}.png'), dpi=300)
    plt.close()

    # Count the occurrences of each rules length
    rules_length_counts = {}
    for length in rules_lengths:
        if length in rules_length_counts:
            rules_length_counts[length] += 1
        else:
            rules_length_counts[length] = 1

    # Create a bar chart for rules length counts
    plt.figure(figsize=(10, 6))
    plt.bar(rules_length_counts.keys(), rules_length_counts.values())
    plt.xlabel('Rules Length')
    plt.ylabel('Count')
    plt.title(f'Number of Rule Sets by Rules Length')
    plt.show()

    plt.savefig(os.path.join(SAVE_PATH, f'rule_sets_by_length.png'), dpi=300)
    plt.close()


DATA_PATH = os.path.join('..', 'Data')
SAVE_PATH = os.path.join(DATA_PATH, 'PartD_Data')
THRESHOLD = 8
def main():
    if not os.path.exists(DATA_PATH):
        print(f"Data path not found: {DATA_PATH}")
        return 1
    
    # Get the file path
    exactmatch_path = os.path.join(DATA_PATH, 'PartA_Data', 'parta_data_by_exactmatch.json')
    if not os.path.exists(exactmatch_path):
        print(f"File not found: {exactmatch_path}")
        return 1

    substrings_paths = []
    partb_data_path = os.path.join(DATA_PATH, 'PartB_Data')
    if not os.path.exists(partb_data_path):
        print(f"Directory not found: {partb_data_path}")
        return 1
    for root, dirs, files in os.walk(partb_data_path):
        for file in files:
            if file.endswith('substrings.json'):
                substrings_paths.append(os.path.join(root, file))

    if not substrings_paths:
        print("No substrings.json files found in PartB_Data/")
        return 1

    
    genPlots(exactmatch_path)
    
if __name__ == "__main__":
    # Set the cwd to the script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    main()