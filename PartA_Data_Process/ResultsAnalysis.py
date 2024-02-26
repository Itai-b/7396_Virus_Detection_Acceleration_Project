"""
This module is in charge of the data prossesing after parsing the snort rules.
"""

import os
import matplotlib.pyplot as plt
import numpy as np
from collections import Counter



"""___________________________________GLOBALS______________________________________"""

plt.style.use('tableau-colorblind10')
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
plt.figure(figsize=(10, 6))

global  total_rules, \
        rules_sid, \
        total_pcre, \
        total_signatures, \
        total_content, \
        total_exactmatches, \
        unique_exactmatches
        
total_rules = 0
rules_sid = []
total_pcre = 0
total_content = 0
total_exactmatches = 0
unique_exactmatches = 0


def plot_lost_rules_by_exactmatch_length(data_by_exactmatch, abs_save_path):
    """
        A function used to plot the cumulated lost rules by the exactmatch length.
    """
    lost_rules_by_length = {}
    for rule in rules_sid:
        lost_rules_by_length[rule] = 0

    for line in data_by_exactmatch:
        for rule in line["rules"]:
            if lost_rules_by_length[rule] <= len(line["exact_match"]):
                lost_rules_by_length[rule] = (len(line["exact_match"]) + 1)
    length = [i for i in range(1, max(lost_rules_by_length.values()) + 1)]
    values_list = list(lost_rules_by_length.values())
    length_counts = [values_list.count(i) for i in range(1, max(lost_rules_by_length.values()) + 1)]
    cumulative_counts = np.cumsum(length_counts)
    
    total_count = cumulative_counts[-1]
    cumulative_percentages = (cumulative_counts / (total_rules - len(rules_with_no_signatures))) * 100
    
    fig, ax1 = plt.subplots()
    color = 'black'
    ax1.set_xlabel('Length of ExactMatch')
    ax1.set_ylabel('Lost Rules', color=color)
    ax1.tick_params(axis='y', labelcolor=color)
    ax1.plot(length, cumulative_counts, color=color)

    ax2 = ax1.twinx()
    ax2.set_ylabel('Percentage (%)', color='black')
    ax2.plot(length, cumulative_percentages, color=colors[1])
    ax2.tick_params(axis='y', labelcolor='black')
    xticks = [2**i for i in range(int(np.log2(max(length))) + 1)]

    for x_val in xticks:
        y_val = cumulative_percentages[x_val-1]
        plt.plot(x_val, y_val, 'o', color=colors[0], markersize=5)
        if x_val == 1:
            plt.text(x_val, y_val-3, f'({x_val}, {y_val:.2f}%)', fontsize=8, fontweight='bold', ha='left', va='bottom')
        else:
            plt.text(x_val, y_val, f'({x_val}, {y_val:.2f}%)', fontsize=8, fontweight='bold', ha='left', va='bottom')

    ax2.tick_params(axis='y', labelcolor=color)
    
    plt.title('Lost Rules by ExactMatch length threshold (only rules with signatures)', fontweight='bold', fontsize=10)
    plt.savefig(os.path.join(abs_save_path, 'cumulative_plot_lost_rules.png'), dpi=300)
    plt.close()  

def plot_cummulative_exactmatch_length(data_by_exactmatch, abs_save_path):
    """
        A function used to plot the cumulative plot of the exact_match lengths.
    """
    
    # Plot the cumulative plot of the exact_match lengths.
    exactmatches_lengths = [len(line["exact_match"]) for line in data_by_exactmatch]
    
    length_counts = [exactmatches_lengths.count(i) for i in range(1, max(exactmatches_lengths) + 1)]
    length = [i for i in range(1, len(length_counts) + 1)]

    cumulative_counts = np.cumsum(length_counts)
    total_count = cumulative_counts[-1]
    cumulative_percentages = (cumulative_counts / total_count) * 100

    fig, ax1 = plt.subplots()
    color = 'black'
    ax1.set_xlabel('Length of ExactMatch')
    ax1.set_ylabel('Count', color=color)
    ax1.tick_params(axis='y', labelcolor=color)
    ax1.plot(length, cumulative_counts, color=color)

    ax2 = ax1.twinx()
    color = 'black'
    ax2.set_ylabel('Percentage (%)', color=color)
    ax2.plot(length, cumulative_percentages, color=colors[1])
    ax2.tick_params(axis='y', labelcolor=color)
    
    xticks = [2**i for i in range(int(np.log2(max(length))) + 1)]
    for x_val in xticks:
        y_val = cumulative_percentages[x_val-1]
        plt.plot(x_val, y_val, 'o', color=colors[0], markersize=5)
        plt.text(x_val, y_val, f'({x_val}, {y_val:.2f}%)', fontsize=8, fontweight='bold', ha='left', va='bottom')

    ax2.tick_params(axis='y', labelcolor=color)
    
    plt.title('Cumulated ExactMatches by length', fontweight='bold')
    plt.savefig(os.path.join(abs_save_path, 'cumulative_plot_exactmatches.png'), dpi=300)
    plt.close()  
    
def check_rules_with_no_signitures(data_by_signature, logger):
    """
        An auxiliary function used to check if there are any rules which do not contain a signature.
    """
    global rules_with_no_signatures
    rules_with_no_signatures = rules_sid.copy()
    for line in data_by_signature:
        for rule in line["rules"]:
            if rule in rules_with_no_signatures:
                rules_with_no_signatures.remove(rule)
    
    if rules_with_no_signatures:
        logger.warning(f'The following rules do not have a content/pcre signature at all: {rules_with_no_signatures}')
    else:
        logger.info(f'All rules still exist after the parsing')
        
def check_rules_lost_while_parsing(data_by_exactmatch, logger):
    """
        An auxiliary function used to find which rules where lost during 
    """
    global rules_lost_while_parsing
    rules_lost_while_parsing = rules_sid.copy()

    for line in data_by_exactmatch:
        if (len(line["exact_match"]) == 0):
            continue
        for rule in line["rules"]:
            if (rule in rules_lost_while_parsing):
                rules_lost_while_parsing.remove(rule)
    
    for rule in rules_with_no_signatures:
        rules_lost_while_parsing.remove(rule)

    if rules_lost_while_parsing:
        logger.warning(f'The following rules were lost during the parsing: {rules_lost_while_parsing}')
    else:
        logger.info(f'All rules with signatures still exist after the parsing')

def log_info(start_time, end_time,logger):
    width = os.get_terminal_size().columns
    
    print('-' * width)
    logger.info('General information after the script\'s execution:')
    logger.info(f'The script\'s execution took {end_time - start_time:.3f} seconds.')
    
    logger.info(f'The snort rule file contains {total_rules} rules.')
    logger.info(f'{len(rules_with_no_signatures)} rules do not have signatures to parse which is {(len(rules_with_no_signatures) / total_rules) * 100 :.2f}% of all the rules.')

    
    logger.info(f'There are {total_content} unique content signatures and {total_pcre} unique pcre signatures in the file.')
    
    logger.info(f'The total number of exactmatches extracted is {total_exactmatches}.')
    logger.info(f'{unique_exactmatches} unique exactmatches were extracted which is {(unique_exactmatches/total_exactmatches) * 100 :.2f}% of all exactmatches.')
    print('-' * width + '\n')

def main(data_by_signature, data_by_exactmatch, abs_save_path, logger):
    global rules_with_no_signatures, \
           rules_lost_while_parsing 
   
    check_rules_with_no_signitures(data_by_signature, logger)
    check_rules_lost_while_parsing(data_by_exactmatch, logger)
    plot_cummulative_exactmatch_length(data_by_exactmatch, abs_save_path)
    plot_lost_rules_by_exactmatch_length(data_by_exactmatch, abs_save_path)
    
