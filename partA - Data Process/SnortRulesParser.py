# Extract rules or sub-strings (for exact match) from Snort's rule set.
# Rules are taken from: https://www.snort.org/downloads/#rule-downloads
# Rule = tuple (line# in src rule file, pattern_type, rule)

import re
import os
import sys

# the () signs separates the string that matched the regex into different groups
# the (?:) indicates a matched group that won't be counted, hence, only groups without '?:' will be counted,
#   but the regex will still match (and discard) the rest.
EXACT_MATCH_RULE_PATTERN = r'(?:content:")(.*?)(?:")'  # exact match rules
REGEX_RULE_PATTERN = r'(?:pcre:")(.*?)(?:")'           # perl compatible regular expression rules

"""
    Parse a file for specified patterns and extract matching data.

    Args:
        file_name (str): The name of the file to parse.
        patterns (dict): A dictionary of pattern names and corresponding regex patterns.

    Returns:
        list[tuple[int, str, str]]: A list of tuples containing line number, matched pattern key,
                                    and the extracted value. If no match, the entire list value is None.

    Example:
        patterns = {
            "Version": r'\d+\.\d+',
            "Name": r'Name:\s+(.*)'
        }
        result = parse_file('data.txt', patterns)
        # Sample output: [(1, 'Version', '1.2'), (2, 'Name', 'John Doe'), ...]
"""


def parse_file(file_name: str, patterns: dict) -> list(tuple([int, str, str])):
    with open(file_name, 'r') as file:
        lines = file.readlines()

    rules = []
    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:
                    # group(1) is the first matching groups (regex in '()'), ignoring groups with '(?:)'
                    data = match.group(1)
                    rule = (line_num, pattern_name, data)
                    rules.append(rule)
    return rules


"""
    An auxiliary function used to print rules in new lines.
"""


def print_rules(rules: list(tuple([int, str, str]))):
    for rule in rules:
        print(rule)


"""
    Usage: python SnortRuleParser.py snort3-community.rules
"""


def main():
    file_path = sys.argv[1]
    patterns = {'content': EXACT_MATCH_RULE_PATTERN,
                'pcre': REGEX_RULE_PATTERN}
    rules = parse_file(file_path, patterns)
    print_rules(rules)
    # TODO: split all 'pcre' rules to exact match (=ignore special regex characters)
    # TODO: discuss how to store content of type: |00 00 00 28 72 24|
    # TODO: save all findings to a file


if __name__ == "__main__":
    # Change working directory to script's dir.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
