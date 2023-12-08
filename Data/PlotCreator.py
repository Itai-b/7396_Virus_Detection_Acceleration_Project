
import sys
import os

from matplotlib import pyplot as plt
from matplotlib.ticker import FuncFormatter


def draw_graphs(length_histogram):
    lengths = sorted(length_histogram.keys())
    counts = [0] * len(lengths)

    for length, num_sub_strings in length_histogram.items():
        for i, value in enumerate(lengths):
            if length <= value:
                counts[i] += num_sub_strings
    
    total_strings = sum(counts)
    cumulative_counts = [sum(counts[:i+1]) / total_strings * 100 for i in range(len(counts))]

    plt.plot(lengths, cumulative_counts, color='green', marker='o')
    plt.gca().yaxis.set_major_formatter(FuncFormatter(lambda x, _: f'{x:.0f}%'))
    plt.yticks([0, 25, 50, 75, 100])
    plt.xlabel('Max String Length')
    plt.ylabel('Strings Presentage')
    plt.title('String Length Histogram')
    plt.show()
    


    lengths = list(length_histogram.keys())
    frequencies = list(length_histogram.values())

    plt.hist(lengths, bins=len(lengths), weights=frequencies, edgecolor='black', alpha=0.7)
    plt.xlabel('String Length')
    plt.ylabel('Frequency')
    plt.title('String Length Histogram')
    plt.show()

    bins = [2, 4, 8, 16, 32, 64, float('inf')]
    counts = [0] * len(bins)

    for length, num_sub_strings in length_histogram.items():
        for i, bin_value in enumerate(bins):
            if length <= bin_value:
                counts[i] += num_sub_strings

    plt.bar(range(len(bins)), counts, align='center')
    plt.xticks(range(len(bins)), [f"<{bins[i]}" if i < len(bins)-1 else f"ALL" for i in range(len(bins))])
    plt.xlabel('Length')
    plt.ylabel('Number of Substrings with Length chars or less')
    plt.title('Number of Substrings with Length chars or less\nfor sub exact matches extracted from Snort rules')
    plt.show()

    max_count = max(counts)
    percentages = [(x / max_count) * 100 for x in counts]
    
    plt.bar(range(len(bins)), percentages, align='center', color='orange')
    plt.xticks(range(len(bins)), [f"<{bins[i]}" if i < len(bins)-1 else f"ALL" for i in range(len(bins))])
    plt.gca().yaxis.set_major_formatter(FuncFormatter(lambda x, _: f'{x:.0f}%'))
    plt.yticks([0, 25, 50, 75, 100])
    plt.ylabel('% of Substrings with Length chars or less')
    plt.title('Percentage of Substrings with Length chars or less\nfor sub exact matches extracted from Snort rules')
    plt.show()

def main():

    length_histogram = {}
    with open("length_histogram.csv", 'r') as f:
        for line in f:
            if (line == "String Length,Count\n") or (line == "\n"):
                continue
            length = int(line.split(',')[0])
            if length not in length_histogram:
                length_histogram[length] = 0
            length_histogram[length] += 1

    draw_graphs(length_histogram)

if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()