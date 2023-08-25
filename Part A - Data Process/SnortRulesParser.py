# Extract rules or sub-strings (for exact match) from Snort's rule set.
# Rules are taken from: https://www.snort.org/downloads/#rule-downloads
# Rule = tuple (line# in src rule file, pattern_type, rule)

import sys
import os
import re
import json
import ExactMatchExtractor as ExactMatchExtracter

# the () signs separates the string that matched the regex into different groups
# the (?:) indicates a matched group that won't be counted, hence, only groups without '?:' will be counted,
#   but the regex will still match (and discard) the rest.
# group(1) is the first matching groups (regex pattern in '()'), ignoring groups with '(?:)'

EXACT_MATCH_RULE_PATTERN = r'(?:content:")(.*?)(?:")'  # exact match rules
REGEX_RULE_PATTERN = r'(?:pcre:")(.*?)(?:")'           # perl compatible regular expression rules


def parse_file(file_name: str, patterns: dict) -> list(tuple([int, str, str])):
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
    
    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:                       
                    data = match.group(1)
                    if (data in previous_data):
                        continue
                    previous_data.append(data)
                    if pattern_name == 'pcre':
                        data = ExactMatchExtracter.run(data, 'raw')
                    rule = (line_num+1, pattern_name, data)
                    rules.append(rule)

    return rules


def save_rules_as_json(exact_matches: list(tuple([int, str, str]))):
    """
        An auxiliary function used to save exact_matches to a json file.
    """
    with open('exact_matches.json', 'w') as file:
        json.dump(exact_matches, file)
           

def print_rules(exact_matches: list(tuple([int, str, str]))):
    """
        An auxiliary function used to print exact_matches in new lines.
    """
    for exact_match in exact_matches:
        print(exact_match)


def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    file_path = ""
    if (__debug__):
        file_path = 'snort3-community.rules'    
    else:
        file_path = sys.argv[1]
    
    patterns = {'content': EXACT_MATCH_RULE_PATTERN,
                'pcre': REGEX_RULE_PATTERN}
    exact_matches = parse_file(file_path, patterns)
    
    if __debug__:
        print_rules(exact_matches)
    else:
        save_rules_as_json(exact_matches)


if __name__ == "__main__":
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
