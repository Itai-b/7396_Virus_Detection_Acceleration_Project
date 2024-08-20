import os
import json
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']

def plot_statistics(abs_test_path, table_data):

    dir_name = os.path.basename(abs_test_path)
    substring_length = dir_name.split('_')[0][-1]
    substring_gap = dir_name.split('_')[1][-1]

    #Create a bar plot for the table statistics
    hash_table_size = []
    average_run_time = []
    load_factor = []
    percentage_of_rules_inserted = []
    percentage_of_all_substrings_inserted = []
    
    # Extract data
    for obj in table_data:
        hash_table_size.append(obj['hash_table_size'])
        average_run_time.append(obj['average_run_time'])
        load_factor.append(obj['load_factor'])
        percentage_of_rules_inserted.append(obj['percentage_of_rules_inserted'])
        percentage_of_all_substrings_inserted.append(obj['percentage_of_all_substrings_inserted'])

    # Define the width of the bars and the positions of the bars for each parameter
    width = 0.2
    x = np.arange(len(hash_table_size))

    fig, ax = plt.subplots(figsize=(10, 6))

    # Plot the data
    rects1 = ax.bar(x - width, percentage_of_rules_inserted,
                    width, label='percentage_of_rules_inserted',
                    color=colors[0])
    rects2 = ax.bar(x, percentage_of_all_substrings_inserted,
                    width, label='percentage_of_all_substrings_inserted',
                    color=colors[1])
    rects3 = ax.bar(x + width, load_factor,
                    width, label='load_factor',
                    color=colors[2])

    ax.set_title('Statistics by Hash Table size using length=' + substring_length + ' and gap=' + substring_gap,
                 fontsize=10, fontweight='bold') 
    ax.set_xticks(x)
    ax.set_xticklabels([f'{size}[KB]' for size in hash_table_size], fontsize=8)
    ax.set_yticklabels([])
    ax.legend()

    # Add average_run_time as text under each hash_table_size on the x-axis
    for i, v in enumerate(average_run_time):
        ax.text(i, -0.1, f'avg_run_time=\n{str(v)}[ms]', fontsize=6, color=colors[3], ha='center', transform=ax.get_xaxis_transform())

    # Function to auto label the bars with their values
    def autolabel(rects):
        for rect in rects:
            height = rect.get_height()
            ax.annotate('{:.2f}'.format(height),
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=6)

    # Call the function for each set of bars
    autolabel(rects1)
    autolabel(rects2)
    autolabel(rects3)

    fig.tight_layout()
    plt.savefig(os.path.join(abs_test_path, 'statistics_by_hash_table_size.png'), dpi=300)
    plt.close()

def main():
    # PartB_Data directory
    data_dir = os.path.abspath("../Data/PartB_Data")
    
    # Get the list of JSON files from the directory
    tests_dir = [dir for dir in os.listdir(data_dir) if os.path.isdir(os.path.join(data_dir, dir))]
    
    for test_dir in tests_dir:
        abs_test_path = os.path.join(data_dir, test_dir)
        hash_table_json_file = next((os.path.join(root, file) for root, dirs, files in os.walk(abs_test_path) for file in files if file.endswith("table_size.json")), None)
    
        if (hash_table_json_file == None):
            print("Missing hash_table JSON files in directory: " + abs_test_path)
            continue
        
        table_data = {}

        # Load the JSON files
        with open(hash_table_json_file, "r") as f:
            table_data = json.load(f)


        plot_statistics(abs_test_path, table_data)

if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    print(dirname)
    os.chdir(dirname)
    main()