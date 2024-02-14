"""
This module is in charge of the data prossesing after parsing the snort rules.
"""

import os
import SnortRulesParser as SnortRulesParser
import matplotlib.pyplot as plt
import numpy as np
from collections import Counter

"""___________________________________GLOBALS______________________________________"""

global  total_rules, \
        total_pcre, \
        total_signatures, \
        total_content, \
        total_exactmatches, \
        unique_exactmatches
        
total_rules = 0
total_pcre = 0
total_content = 0
total_exactmatches = 0
unique_exactmatches = 0

def create_plots(data_by_signature, data_by_exactmatch, abs_save_path, logger):
    """
        A function used to create the plots for the analysis.
    """
    
    # Plot the cumulative plot of the exact_match lengths.
    exactmatches_lengths = [len(line["exact_match"]) for line in data_by_exactmatch]
    length_counts = dict(Counter(exactmatches_lengths))
    
    sorted_lengths = sorted(length_counts.keys())
    sorted_counts = [length_counts[length] for length in sorted_lengths]

    cumulative_counts = np.cumsum(sorted_counts)

    total_count = cumulative_counts[-1]
    cumulative_percentages = (cumulative_counts / total_count) * 100

    fig, ax1 = plt.subplots()

    color = 'tab:blue'
    ax1.set_xlabel('Length of ExactMatch Substring')
    ax1.set_ylabel('Count', color=color)
    ax1.plot(sorted_lengths, cumulative_counts, color=color)
    ax1.tick_params(axis='y', labelcolor=color)

    ax2 = ax1.twinx()
    color = 'tab:red'
    ax2.set_ylabel('Percentage (%)', color=color)
    ax2.plot(sorted_lengths, cumulative_percentages, color=color)
    ax2.tick_params(axis='y', labelcolor=color)

    plt.title('Cumulative plot of ExactMatch Substrings lengths')

    plt.savefig(os.path.join(abs_save_path, 'cumulative_plot_exactmatches.png'), dpi=300)
    logger.info(f'Saved the cumulative plot of exactmatches lengths under the path {abs_save_path}')
    
def check_rules_with_no_signitures(data_by_signature, logger):
    """
        An auxiliary function used to check if there are any rules which do not contain a signature.
    """
    global rules_with_no_signatures
    rules_with_no_signatures = list(range(1, total_rules + 1))
    for line in data_by_signature:
        for rule in line["rules"]:
            if rule in rules_with_no_signatures:
                rules_with_no_signatures.remove(rule)
    
    if rules_with_no_signatures:
        logger.warning(f'The following rules do not have a content/pcre signature at all: {rules_with_no_signatures}')
    else:
        logger.info(f'All rules still exist after the parsing')
        
def check_rules_lost_while_parsing(data_by_signature, logger):
    """
        An auxiliary function used to find which rules where lost during 
    """
    global rules_lost_while_parsing
    rules_lost_while_parsing = list(range(1, total_rules + 1))
    for line in data_by_signature:
        if (len(line["exact_matches"]) == 0):
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
    logger.info('General information after the script\'s execution:\n')
    logger.info(f'The script\'s execution took {end_time - start_time:.3f} seconds.')
    
    logger.info(f'The snort rule file contains {total_rules} rules.')
    logger.info(f'{len(rules_with_no_signatures)} rules do not have signatures to parse which is {(len(rules_with_no_signatures) / total_rules) * 100 :.2f}% of all the rules.')

    
    logger.info(f'There are {total_content} content signatures and {total_pcre} pcre signatures in the file.')
    
    logger.info(f'The total number of exactmatches extracted is {total_exactmatches}.')
    logger.info(f'{unique_exactmatches} unique exactmatches were extracted which is {(unique_exactmatches/total_exactmatches) * 100 :.2f}% of all exactmatches.')
    print('-' * width + '\n')

def main(data_by_signature, data_by_exactmatch, abs_save_path, logger):
    global rules_with_no_signatures, \
           rules_lost_while_parsing 
   
    check_rules_with_no_signitures(data_by_signature, logger)
    check_rules_lost_while_parsing(data_by_signature, logger)
    create_plots(data_by_signature, data_by_exactmatch, abs_save_path, logger)
    
