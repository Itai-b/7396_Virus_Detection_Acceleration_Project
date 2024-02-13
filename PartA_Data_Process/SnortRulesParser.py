"""
TODO: update documentation.

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
import ResultsAnalysis as ResultsAnalysis
from datetime import datetime
from config import config

logging.basicConfig(level=logging.INFO)

# Create a FileHandler to save log messages to a file
log_file_handler = logging.FileHandler('SnortRuleParser.log')

# Clear the log file from previous runs
with open('SnortRuleParser.log', 'w') as file:
    pass

logger = logging.getLogger('SnortRulesParser')

EXACT_MATCH_SIGNATURE = r'(?:content:")(.*?)(?:")'  # exact match rules
REGEX_SIGNATURE = r'(?:pcre:")(.*?)(?:")'           # perl compatible regular expression rules

def check_duplicate_signatures(signature, signature_type, rule_number, data):
    if not data:
        data.append({"signature": signature, "signature_type": signature_type, "rules": [rule_number]})
        return False
    
    for line in data:
        if signature == line["signature"]:
            line["rules"].append(rule_number)
            return True
    data.append({"signature": signature,"signature_type": signature_type, "rules": [rule_number]})
    return False

def add_exact_matches_to_data(signature, exact_matches, exact_matches_hex, data_by_signature):
    for line in data_by_signature:
        if signature == line["signature"]:
            line["exact_matches"] = exact_matches
            line["exact_matches_hex"] = exact_matches_hex
            return

def add_data_by_exactmatch(exact_matches, exact_matches_hex, rule, signature_type, data_by_exactmatch):
    for i, exact_match in enumerate(exact_matches):
        for line in data_by_exactmatch:
            if (exact_match == line["exact_match"]):
                if rule not in line["rules"]:
                    line["rules"].append(rule)
                if signature_type not in line["signature_type"]:
                    line["signature_type"].append(signature_type)
                return
        data_by_exactmatch.append({"exact_match": exact_match, "exact_match_hex": exact_matches_hex[i], "signature_type": [signature_type], "rules": [rule]})
    

def save_data_as_json(data_by_signature, data_by_exactmatch, save_path):
    """
        An auxiliary function used to save part_a data to a json file.
    """
    json_path = os.path.join(save_path, 'parta_data_by_signature.json')
    with open(json_path, 'w') as file:
        for line in data_by_signature:
            json.dump(line, file, indent=None)
            file.write('\n')
            
    json_path = os.path.join(save_path, 'parta_data_by_exactmatch.json')
    with open(json_path, 'w') as file:
        for line in data_by_exactmatch:
            json.dump(line, file, indent=None)
            file.write('\n')
             
    logger.info(f"Saved the partA data as .json file under the path {save_path}")

def parse_file(file_name, signatures):
    
    with open(file_name, 'r') as file:
        rules = file.readlines()
    
    data_by_signature = []
    data_by_exactmatch = []
    
    ResultsAnalysis.total_rules = len(rules)
    
    for rule_num, rule in enumerate(rules):
        for signature_type, signature in signatures.items():
            matches = re.finditer(signature, rule)
            if matches:
                for match in matches:                       
                    signature = match.group(1)
                    
                    if check_duplicate_signatures(signature, signature_type, rule_num+1, data_by_signature):
                        continue
                    
                    exact_matches = ""
                    if signature_type == 'pcre':
                        ResultsAnalysis.total_pcre += 1
                        exact_matches = ExactMatchExtractor.run(signature, 'char')
                    else:   # signature_type == 'content':
                        ResultsAnalysis.total_content += 1
                        exact_matches = ContentProcessor.run(signature.lower())
                    
                    exact_matches_hex = translate_exact_matches_to_hex(exact_matches)
                    add_exact_matches_to_data(signature, exact_matches, exact_matches_hex, data_by_signature)
                    add_data_by_exactmatch(exact_matches, exact_matches_hex, rule_num + 1, signature_type, data_by_exactmatch)
                    ResultsAnalysis.total_exactmatches += len(exact_matches)

    return data_by_signature, data_by_exactmatch

def translate_exact_matches_to_hex(exact_matches):
    """
        An auxiliary function used to translate exact_matche to hex.
    """
    
    exact_matches_hex = []
    for exact_match in exact_matches:
        hex_list = []
        for i, char_list in enumerate(exact_match):
            for char in char_list:
                hex_list.append(hex(ord(char)))
        exact_matches_hex.append(hex_list)
    return exact_matches_hex            


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

    abs_save_path = os.path.abspath(args.path)
    logger.info(f'The path where the files will be saved is {abs_save_path}.')

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        # Create a formatter to specify the format of log messages
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        log_file_handler.setFormatter(formatter)
        logger.addHandler(log_file_handler)
        
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
    
    data_by_signature, data_by_exact_match = parse_file(file_path, signatures_type)
    ResultsAnalysis.unique_exactmatches = len(data_by_exact_match)    

    ResultsAnalysis.main(data_by_signature, logger)
    
    if args.json:
        save_data_as_json(data_by_signature, data_by_exact_match, abs_save_path)
        
    logger.info(f'Finished parsing the file.\n')
    end_time = time.time()
    ResultsAnalysis.log_info(start_time, end_time, logger)


if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
