"""
This module is in charge of the data prossesing after parsing the snort rules.
"""

from math import log
import os
import csv
from config import config


"""___________________________________GLOBALS______________________________________"""

global  total_rules, \
        total_pcre, \
        total_signatures, \
        total_content, \
        total_exactmatches
        
total_rules = 0
total_pcre = 0
total_content = 0
total_exactmatches = 0

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
    #logger.info(f'The script used the following threshold: {config.MINIMAL_EXACT_MATCH_LENGTH} for exact matches.\n')
    
    logger.info(f'The snort rule file contains {total_rules} rules.')
    logger.info(f'{len(rules_with_no_signatures)} rules do not have signatures to parse which is {(len(rules_with_no_signatures) / total_rules) * 100 :.2f}% of all the rules.')
    logger.info(f'{total_rules - len(rules_lost_while_parsing)} rules with signatures left after parsing which is {((total_rules - len(rules_lost_while_parsing)) / total_rules) * 100 :.2f}% of them.\n')

    
    logger.info(f'There are {total_content} content signatures and {total_pcre} pcre signatures in the file.')
    
    logger.info(f'The total number of exactmatches extracted is {total_exactmatches}.')
    print('-' * width + '\n')

def main(data_by_signature, logger):
    global rules_with_no_signatures, \
           rules_lost_while_parsing 
    
    check_rules_with_no_signitures(data_by_signature, logger)
    check_rules_lost_while_parsing(data_by_signature, logger)
