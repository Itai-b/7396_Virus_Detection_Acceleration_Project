# Extract rules or sub-strings (for exact match) from Snort's rule set.
# Rules are taken from: https://www.snort.org/downloads/#rule-downloads
# Rule = tuple (line# in src rule file, pattern_type, rule)

from codecs import utf_8_encode
import re
import os
import sys
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
    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:
                    data = match.group(1)
                    if pattern_name == 'pcre':
                        data = ExactMatchExtracter.run(data)
                    rule = (line_num, pattern_name, data)
                    rules.append(rule)

    return rules


def print_rules(rules: list(tuple([int, str, str]))):
    """
        An auxiliary function used to print rules in new lines.
    """
    for rule in rules:
        print(rule)


def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    #file_path = sys.argv[1]
    file_path = 'snort3-community.rules'
    patterns = {'content': EXACT_MATCH_RULE_PATTERN,
                'pcre': REGEX_RULE_PATTERN}
    rules = parse_file(file_path, patterns)
    print_rules(rules)
    # TODO: split all 'pcre' rules to exact match (=ignore special regex characters)
    # TODO: discuss how to store content of type: |00 00 00 28 72 24|
    # TODO: save all findings to a file


if __name__ == "__main__":
    # Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
