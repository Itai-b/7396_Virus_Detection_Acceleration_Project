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

Description:
    This script parses a given rule file for specified patterns and extracts matching data.
    It supports extracting exact match rules and Perl compatible regular expression rules.
    Extracted rules are saved as a JSON file or printed to the console.

Modules:
    - sys
    - os
    - re (Regular Expressions)
    - json
    - ExactMatchExtractor (Custom module for exact match extraction)

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

    - main():
        The main function that orchestrates the parsing and processing of rules.
"""

import sys
import os
import re
import json
import ExactMatchExtractor as ExactMatchExtractor
import ContentProcessor as ContentProcessor
from config import config

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
    global relevant_content, total_content, relevant_pcre, total_pcre, special_R_rules

    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:                       
                    data = match.group(1)
                    if data in previous_data:
                        continue
                    previous_data.append(data)
                    if pattern_name == 'pcre':
                        total_pcre += 1
                        data = ExactMatchExtractor.run(data, 'char') # TODO: function records \R rules.
                    else:   # pattern_name == 'content':
                        total_content += 1
                        data = data.lower()
                        data = ContentProcessor.run(data)
                    #TODO: add config.THRESHOLD for a minimal char length of a substring.
                    #TODO: USE HERE RETURN VALUE FROM FUNCTION (EMPTY LIST = DISCARD RULE)
                    data = analyze_and_threshold(data)
                    if data:
                        rule = (line_num+1, pattern_name, data) # TODO: add max substring length
                        rules.append(rule)
                        if pattern_name == 'pcre':
                            relevant_pcre += 1
                        else:   # content
                            relevant_content += 1
                    else:   # data was either empty or thresholded
                        # TODO: add to json log of discarded rules
                        continue


    return rules


#TODO: make statistic class instead of using global params
def analyze_and_threshold(data: list) -> list:
    thresholded_data = []
    global total_sub_strings, relevant_sub_strings, length_histogram

    for char_list in data:
        # print(f"char_list = {char_list}: length = {len(char_list)}")
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
    
    with open('exact_matches.json', 'w') as file:
        for exact_match in exact_matches:
            json.dump(exact_match, file, indent=None)
            file.write('\n')
            
    print(f"saved the exact-matches as .json file under the path {os.getcwd()}\exact_matches.json")
           

def print_exact_matches(exact_matches: list(tuple([int, str, list]))):
    """
        An auxiliary function used to print exact_matches in new lines.
    """
    # print(f"char_list = {char_list}: length = {len(char_list)}")
    for exact_match in exact_matches:
        print(exact_match)


total_sub_strings = 0
relevant_sub_strings = 0

total_pcre = 0
relevant_pcre = 0
special_R_rules = 0

total_content = 0
relevant_content = 0

length_histogram = {}


def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    global total_sub_strings,\
        relevant_sub_strings, \
        total_pcre, \
        relevant_pcre, \
        special_R_rules, \
        total_content, \
        relevant_content, \
        length_histogram

    total_sub_strings = 0
    relevant_sub_strings = 0
    total_pcre = 0
    relevant_pcre = 0
    special_R_rules = 0
    total_content = 0
    relevant_content = 0
    length_histogram = {}

    save_as_json = False  # Initialize the flag

    if '-json' in sys.argv:
        save_as_json = True
        sys.argv.remove('-json')  # Remove the flag from the arguments list


    file_path = ""
    if len(sys.argv) == 1:
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

    length_histogram = sorted(length_histogram.items(), key=lambda x:x[0])
    for item in length_histogram:
        print(f"Length {item[0]}: {item[1]} strings.")
    print(f"{relevant_sub_strings/total_sub_strings * 100 :.2f}% substrings remained after thresholding t = {config.MINIMAL_EXACT_MATCH_LENGTH}.")
    print(f"{(relevant_pcre + relevant_content) / (total_pcre + total_content) * 100 :.2f}% rules remained.")
    print(f"{relevant_pcre / total_pcre * 100 :.2f}% pcre rules remained.")
    print(f"{relevant_content / total_content * 100 :.2f}% content rules remained.")
    # print(f"{special_R_rules} out of {total_pcre} pcre rules were \\R rules.")


if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
