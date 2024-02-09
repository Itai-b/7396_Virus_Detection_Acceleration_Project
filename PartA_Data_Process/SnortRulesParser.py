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
    This script parses a given rule file for specified signatures and extracts matching data.
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
    - EXACT_MATCH_SIGNATURE_signature: Regular expression signature for exact match rules.
    - REGEX_SIGNATURE_signature: Regular expression signature for Perl compatible regular expression rules.

Functions:
    - parse_file(file_name: str, signatures: dict) -> list[tuple(int, str, str)]:
        Parse a file for specified signatures and extract matching data.
        
    - save_rules_as_json(exact_matches: list[tuple(int, str, str)]):
        An auxiliary function to save exact_matches to a JSON file.
        
    - print_rules(exact_matches: list[tuple(int, str, str)]):
        An auxiliary function to print exact_matches, each in a new line.
        
    - log_info(start_time, end_time, length_histogram):
        An auxiliary function to log information about the script's execution.

    - main():
        The main function that orchestrates the parsing and processing of rules.
"""

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

# Clear the log file from previous runs
with open('SnortRuleParser.log', 'w') as file:
    pass

# Create a formatter to specify the format of log messages
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
log_file_handler.setFormatter(formatter)

logger = logging.getLogger('SnortRulesParser')

# Add both handlers to the root logger
logger.addHandler(log_file_handler)

EXACT_MATCH_SIGNATURE = r'(?:content:")(.*?)(?:")'  # exact match rules
REGEX_SIGNATURE = r'(?:pcre:")(.*?)(?:")'           # perl compatible regular expression rules

def check_duplicate_signatures(signature, signature_type, rule_number, signatures_hist):
    """
        Check if the substring was already extracted from the file.

        :param substring: A string represents the substring to check.
        :param rule_number: An integer represents the rule number.
        :param substrings: A list of tuples containing line number, matched signature key,
                            and the extracted value.

        :return: A boolean value represents whether the substring was already extracted.
    """
    if not signatures_hist:
        signatures_hist.append({"signature": signature, "signature_type": signature_type, "rules": [rule_number]})
        return False
    
    for data in signatures_hist:
        if signature == data["signature"]:
            data["rules"].append(rule_number)
            return True
    signatures_hist.append({"signature": signature,"signature_type": signature_type, "rules": [rule_number]})
    return False

def add_exact_matches_to_hist(signature, exact_matches, signatures_hist):
    for data in signatures_hist:
        if signature == data["signature"]:
            data["exact_matches"] = exact_matches
            return

def save_signature_hist_as_json(signatures_hist, save_path):
    """
        An auxiliary function used to save exact_matches to a json file.
    """
    json_path = os.path.join(save_path, 'signatures_hist.json')
    with open(json_path, 'w') as file:
        for signature in signatures_hist:
            json.dump(signature, file, indent=None)
            file.write('\n')
             
    logger.info(f"Saved the signatures_hist as .json files under the path {save_path}")

def parse_file(file_name: str, signatures: dict):
    
    with open(file_name, 'r') as file:
        rules = file.readlines()
    
    signatures_hist = []
    global total_rules, lost_rules, relevant_content, total_content, relevant_pcre, total_pcre
    
    total_rules = len(rules)
    
    for rule_num, rule in enumerate(rules):
        for signature_type, signature in signatures.items():
            matches = re.finditer(signature, rule)
            if matches:
                for match in matches:                       
                    data = match.group(1)
                    
                    if check_duplicate_signatures(data, signature_type, rule_num+1, signatures_hist):
                        continue
                    
                    if signature_type == 'pcre':
                        total_pcre += 1
                        data = ExactMatchExtractor.run(data, 'char')
                    else:   # signature_type == 'content':
                        total_content += 1
                        data = data.lower()
                        data = ContentProcessor.run(data)
                    
                    data = analyze_and_threshold(data)
                    add_exact_matches_to_hist(match.group(1), data, signatures_hist)
                    
                    if data:
                        if signature_type == 'pcre':
                            relevant_pcre += 1
                        else:   # content
                            relevant_content += 1
    
    return signatures_hist

def analyze_and_threshold(data: list) -> list:
    thresholded_data = []
    global total_exactmatches, relevant_exactmatches, length_histogram

    for char_list in data:
        total_exactmatches += 1
        length = len(char_list)

        if length in length_histogram:
            length_histogram[length] += 1
        else:
            length_histogram[length] = 1

        if length > config.MINIMAL_EXACT_MATCH_LENGTH:
            relevant_exactmatches += 1
            thresholded_data.append(char_list)

    return thresholded_data

def check_lost_rules(signatures_hist, lost_rules):
    """
        An auxiliary function used to check if there are any lost rules.
    """
    for signature in signatures_hist:
        for rule in signature["rules"]:
            if rule in lost_rules:
                lost_rules.remove(rule)
    if lost_rules:
        logger.warning(f'The following rules were lost during the parsing: {lost_rules}')
    else:
        logger.info(f'All rules still exist after the parsing')

def translate_exact_matches_to_hex(signature_hist):
    """
        An auxiliary function used to translate exact_matches to hex.
    """
    
    for signature in signature_hist:
        for exact_match in signature["exact_matches"]:
            exact_match_hex = []
            for i, char_list in enumerate(exact_match):
                hex_list = []
                for char in char_list:
                    hex_list.append(hex(ord(char)))
                    exact_match_hex.append(hex_list)
            signature["exact_matches_hex"] = exact_match_hex            

def save_exact_matches_as_json(exact_matches: list(tuple([int, str, list])), exact_matches_hex: list(tuple([int, str, list])), save_path):
    """
        An auxiliary function used to save exact_matches to a json file.
    """
    json_path = os.path.join(save_path, 'exact_matches.json')
    with open(json_path, 'w') as file:
        for exact_match in exact_matches:
            json.dump(exact_match, file, indent=None)
            file.write('\n')
            
    json_path_hex = os.path.join(save_path, 'exact_matches_hex.json')
    with open(json_path_hex, 'w') as file:
        for exact_match_hex in exact_matches_hex:
            json.dump(exact_match_hex, file, indent=None)
            file.write('\n')
             
    logger.info(f"Saved the exact-matches as .json files under the path {save_path}")       

def print_exact_matches(exact_matches: list(tuple([int, str, list]))):
    """
        An auxiliary function used to print exact_matches in new lines.
    """
    for exact_match in exact_matches:
        print(exact_match)
        
def log_info(start_time, end_time):
    width = os.get_terminal_size().columns
    
    print('-' * width)
    print('General information after the script\'s execution:')
    print(f'The script\'s execution took {end_time - start_time:.3f} seconds.')
    print(f'The script used the following threshold: {config.MINIMAL_EXACT_MATCH_LENGTH} for exact matches.\n')
    
    print(f'The snort rule file contains {total_rules} rules.')
    print(f'{len(lost_rules)} rules were lost during the parsing which is {(len(lost_rules) / total_rules) * 100 :.2f}% of all the rules.\n')
    
    print(f'There are {total_content} content signatures and {total_pcre} pcre signatures in the file.')
    print(f'{(relevant_pcre + relevant_content) / (total_pcre + total_content) * 100 :.2f}% of the signatures remained after thresholding.')
    print(f'{relevant_pcre / total_pcre * 100 :.2f}% pcre signatures remained remained after thresholding.')
    print(f'{relevant_content / total_content * 100 :.2f}% content signatures remained after thresholding.\n')
    
    print(f'The total number of exactmatches is {total_exactmatches}.')
    print(f'{total_exactmatches - relevant_exactmatches} were removed after thresholding.')
    print(f'{relevant_exactmatches/total_exactmatches * 100 :.2f}% of the exactmatches remained after thresholding.')
    print('-' * width + '\n')
    
def save_data(length_histogram, save_path):
    length_hint_path = os.path.join(save_path, 'length_histogram.csv')
    with open(length_hint_path, 'w', newline='') as csvfile:
        headers = ['String Length', 'Count']
        result_list = []
        for length, count in length_histogram.items():
            result_list.append({'String Length': length, 'Count': count})
        result_list.sort(key=lambda x: x['String Length'])
        writer = csv.DictWriter(csvfile, headers)
        writer.writeheader()
        writer.writerows(result_list)
    
    threshold_hist_path = os.path.join(save_path, 'threshold_histogram.csv')
    with open(threshold_hist_path, 'w', newline='') as csvfile:
        bins = [2, 4, 8, 16, 32, 64, float('inf')]
        counts = [0] * len(bins)
        for length, num_sub_strings in length_histogram.items():
            for i, bin_value in enumerate(bins):
                if length <= bin_value:
                    counts[i] += num_sub_strings
        bins = ['<' + str(bins[i]) if i < len(bins) - 1 else 'ALL' for i in range(len(bins))]

        headers = ['Threshold Size', 'Count']
        result_list = []
        for index, bin in enumerate(bins):
            result_list.append({'Threshold Size':  bin, 'Count': counts[index]})
        writer = csv.DictWriter(csvfile, headers)
        writer.writeheader()
        writer.writerows(result_list)

def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    parser = argparse.ArgumentParser(description="Snort Rule Set Extractor")
    parser.add_argument('file_name', metavar='file_name', type=str, nargs='?', default='snort3-community.rules')
    parser.add_argument('-p', '--path', type=str, default=os.getcwd(), help='The path where to save all the files')
    parser.add_argument('-d', '--debug', action='store_true', help='Prints debug information about the script\'s execution.')
    parser.add_argument('-j', '--json', action='store_true', help='Saves the exact matches as a .json file.')
    
    args = parser.parse_args()

    logger.info(f'Started parsing.')
    start_time = time.time()

    global total_rules, \
        lost_rules, \
        total_pcre, \
        total_signatures, \
        relevant_signatures, \
        relevant_pcre, \
        total_content, \
        relevant_content, \
        total_exactmatches, \
        relevant_exactmatches, \
        length_histogram

    total_rules = 0
    lost_rules = []
    total_pcre = 0
    relevant_pcre = 0
    total_content = 0
    relevant_content = 0
    total_exactmatches = 0
    relevant_exactmatches = 0
    length_histogram = {}

    abs_save_path = os.path.abspath(args.path)
    logger.info(f'The path where the files will be saved is {abs_save_path}.')

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        
    file_path = ""
    if args.file_name:
        file_path = 'snort3-community.rules'    
    else:
        for arg in sys.argv:
            if arg.endswith('.rules'):
                file_path = arg
                break
    
    signatures_type = {'content': EXACT_MATCH_SIGNATURE,
                'pcre': REGEX_SIGNATURE}
    
    signatures_hist = parse_file(file_path, signatures_type)
    
    
    lost_rules = list(range(1, total_rules + 1))
    
    check_lost_rules(signatures_hist, lost_rules)
    translate_exact_matches_to_hex(signatures_hist)
    
    if args.json:
        save_signature_hist_as_json(signatures_hist, abs_save_path)
        
    save_data(length_histogram, abs_save_path)    

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
