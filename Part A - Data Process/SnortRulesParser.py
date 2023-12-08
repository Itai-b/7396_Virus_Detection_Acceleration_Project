"""
Snort Rule Set Extractor

This script extracts rules or sub-strings (for exact match) from Snort's rule set.
Rules are taken from: https://www.snort.org/downloads/#rule-downloads (snort3-community.rules)
There is an option to use any other rule set (which use the same format)

Authors:
    Idan Baruch (idan-b@campus.technion.ac.il)
    Itai Benyamin (itai.b@campus.technion.ac.il)

Usage:
    Run the script from the terminal using the following command:
    python SnortRulesParser.py snort3-community.rules
    - if no file name is given, the script will use the default file name: snort3-community.rules
    - if -json flag is given, the script will save the exact matches as a .json file under the path where the script is located.
    - if -info flag is given, the script will print information about the script's execution to the console.

Description:
    This script parses a given rule file for specified patterns and extracts matching data.
    It supports extracting exact match rules and Perl compatible regular expression rules.
    Extracted rules are saved as a JSON file or printed to the console.

Modules:
    - sys
    - os
    - argparse
    - re (Regular Expressions)
    - logging (For logging information about the script's execution)
    - time
    - datetime
    - json
    - ExactMatchExtractor (Custom module for exact match extraction)
    - ContentProcessor (Custom module for content processing)
    - config (Custom module for configurations and constants)

Constants:
    - EXACT_MATCH_RULE_PATTERN: Regular expression pattern for exact match rules.
    - REGEX_RULE_PATTERN: Regular expression pattern for Perl compatible regular expression rules.

Functions:
    - parse_file(file_name: str, patterns: dict) -> list[tuple(int, str, str)]:
        Parse a file for specified patterns and extract matching data.
        
    - save_rules_as_json(exact_matches: list[tuple(int, str, str)]):
        An auxiliary function to save exact_matches to a JSON file.
        
    - print_rules(exact_matches: list[tuple(int, str, str)]):
        An auxiliary function to print exact_matches, each in a new line.
        
    - log_info(start_time, end_time, length_histogram):
        An auxiliary function to log information about the script's execution.

    - main():
        The main function that orchestrates the parsing and processing of rules.
"""

from itertools import count
import sys
import os
import argparse
import re
import logging
import time
import csv
import json

import ExactMatchExtractor as ExactMatchExtractor
import ContentProcessor as ContentProcessor
from datetime import datetime
from config import config

logging.basicConfig(level=logging.INFO)

# Create a FileHandler to save log messages to a file
log_file_handler = logging.FileHandler('SnortRuleParser.log')

# Create a formatter to specify the format of log messages
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
log_file_handler.setFormatter(formatter)

# Get the root logger
logger = logging.getLogger('')

# Add both handlers to the root logger
logger.addHandler(log_file_handler)

EXACT_MATCH_RULE_PATTERN = r'(?:content:")(.*?)(?:")'  # exact match rules
REGEX_RULE_PATTERN = r'(?:pcre:")(.*?)(?:")'           # perl compatible regular expression rules


def parse_file(file_name: str, patterns: dict) -> list(tuple([int, str, list])):
    """
        Parse a file for specified patterns and extract matching data.

        :param file_name: A string represents the name of the file to parse.
        :param patterns: A dictionary of pattern names and corresponding regex patterns.

        :return: A list of tuples containing line number, matched pattern key,
                    and the extracted value. If no match, the entire list value is None.

        Example:
            patterns = {
                "Version": r'\d+\.\d+',
                "Name": r'Name:\s+(.*)'
            }
            result = parse_file('data.txt', patterns)
            # Sample output: [(1, 'Version', '1.2'), (2, 'Name', 'John Doe'), ...]
    """
    
    with open(file_name, 'r') as file:
        lines = file.readlines()
    
    rules = []
    previous_data = []
    global relevant_content, total_content, removed_content, relevant_pcre, total_pcre, removed_pcre, special_R_rules

    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:                       
                    data = match.group(1)
                    if data in previous_data:
                        if pattern_name == 'pcre':
                            removed_pcre += 1
                        else:  # pattern_name == 'content':
                            removed_content += 1
                        continue
                    previous_data.append(data)
                    if pattern_name == 'pcre':
                        total_pcre += 1
                        data = ExactMatchExtractor.run(data, 'char')
                    else:   # pattern_name == 'content':
                        total_content += 1
                        data = data.lower()
                        data = ContentProcessor.run(data)
                    data = analyze_and_threshold(data)
                    if data:
                        rule = (line_num+1, pattern_name, data)
                        rules.append(rule)
                        if pattern_name == 'pcre':
                            relevant_pcre += 1
                        else:   # content
                            relevant_content += 1
                    else:   # data was either empty or thresholded
                        if pattern_name == 'pcre':
                            special_R_rule = r'\/R[ims]{0,3}$'
                            if re.search(special_R_rule, match.group(1)):
                                special_R_rules += 1
                                logger.info(f"the following {pattern_name} pattern with special R rule in line {line_num+1} was discarded: {match.group(1)}")
                            else:
                                logger.debug(f"the following {pattern_name} pattern in line {line_num+1} was discarded: {match.group(1)}")
    return rules

def analyze_and_threshold(data: list) -> list:
    thresholded_data = []
    global total_sub_strings, relevant_sub_strings, length_histogram

    for char_list in data:
        total_sub_strings += 1
        length = len(char_list)

        if length in length_histogram:
            length_histogram[length] += 1
        else:
            length_histogram[length] = 1

        if length > config.MINIMAL_EXACT_MATCH_LENGTH:
            relevant_sub_strings += 1
            thresholded_data.append(char_list)

    return thresholded_data


def save_exact_matches_as_json(exact_matches: list(tuple([int, str, list]))):
    """
        An auxiliary function used to save exact_matches to a json file.
    """
    
    with open('..\Data\exact_matches.json', 'w') as file:
        for exact_match in exact_matches:
            json.dump(exact_match, file, indent=None)
            file.write('\n')
            
    logger.info(f"Saved the exact-matches as .json file under the path {os.pardir}\Data\exact_matches.json.")
           

def print_exact_matches(exact_matches: list(tuple([int, str, list]))):
    """
        An auxiliary function used to print exact_matches in new lines.
    """
    for exact_match in exact_matches:
        print(exact_match)
        
def log_info(start_time, end_time):
    
    logger.info(f'The script\'s execution took {end_time - start_time:.3f} seconds.')
    logger.info(f'There are {total_content} content rules and {total_pcre} pcre rules in the file.')
    logger.info(f'There are {special_R_rules} special R rules in the file.')
    logger.info(f'There were {removed_pcre} special case pcre rules that were removed.')
    logger.info(f'There were {removed_content} special case content rules that were removed.')

    logger.info('General information after the script\'s execution:')
    logger.info(f'{relevant_sub_strings/total_sub_strings * 100 :.2f}% of the substrings remained after thresholding t = {config.MINIMAL_EXACT_MATCH_LENGTH}.')
    logger.info(f'{(relevant_pcre + relevant_content) / (total_pcre + total_content) * 100 :.2f}% of the rules remained after thresholding.')
    logger.info(f'{relevant_pcre / total_pcre * 100 :.2f}% pcre rules remained remained after thresholding.')
    logger.info(f'{relevant_content / total_content * 100 :.2f}% content rules remained after thresholding.')
    



def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    parser = argparse.ArgumentParser(description="Snort Rule Set Extractor")
    parser.add_argument('file_name', metavar='file_name', type=str, nargs='?', default='snort3-community.rules')
    parser.add_argument('-info', action='store_true', help='Prints information about the script\'s execution.')
    parser.add_argument('-json', action='store_true', help='Saves the exact matches as a .json file.')
    
    args = parser.parse_args()

    logger.info(f'Started parsing.')
    start_time = time.time()

    global total_sub_strings,\
        relevant_sub_strings, \
        total_pcre, \
        relevant_pcre, \
        removed_pcre, \
        special_R_rules, \
        total_content, \
        relevant_content, \
        removed_content, \
        length_histogram

    total_sub_strings = 0
    relevant_sub_strings = 0
    total_pcre = 0
    relevant_pcre = 0
    removed_pcre = 0
    special_R_rules = 0
    total_content = 0
    relevant_content = 0
    removed_content = 0
    length_histogram = {}

    if args.info:
        logging.basicConfig(level=logging.DEBUG)
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
        logger.addHandler(console_handler)

    save_as_json = False  # Initialize the flag

    if args.json:
        save_as_json = True
        sys.argv.remove('-json')  # Remove the flag from the arguments list

    file_path = ""
    if args.file_name:
        file_path = 'snort3-community.rules'    
    else:
        for arg in sys.argv:
            if arg.endswith('.rules'):
                file_path = arg
                break
    
    patterns = {'content': EXACT_MATCH_RULE_PATTERN,
                'pcre': REGEX_RULE_PATTERN}
    
    exact_matches = parse_file(file_path, patterns)
    
    if save_as_json:
        save_exact_matches_as_json(exact_matches)
    else:
        print_exact_matches(exact_matches)
        
    with open('../Data/length_histogram.csv', 'w') as csvfile:
        headers = ['String Length', 'Count']
        result_list = []
        for length, count in length_histogram.items():
            result_list.append({'String Length': length, 'Count': count})
        writer = csv.DictWriter(csvfile, headers)
        writer.writeheader()
        writer.writerows(result_list)
        
    logger.info(f'Finished parsing the file.')
    end_time = time.time()
    log_info(start_time, end_time)


if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
