# Extract rules or sub-strings (for exact match) from Snort's rule set.
# Rules are taken from: https://www.snort.org/downloads/#rule-downloads
# Rule = tuple (line# in src rule file, 'content' / 'pcre', rule)

import re
import os
import sys


EXACT_MATCH_RULE_PATTERN = 'content:".*?"'    # exact match rules
REGEX_RULE_PATTERN = 'pcre:".*?"'             # perl compatible regular expression rules


def parse_file(file_name: str, patterns: dict) -> list[tuple[int, str, str]]:
    with open(file_name, 'r') as file:
        lines = file.readlines()

    rules = []
    for line_num, line in enumerate(lines):
        for pattern_name, pattern in patterns.items():
            matches = re.finditer(pattern, line)
            if matches:
                for match in matches:
                    data = match.group()
                    rule = tuple[line_num, pattern_name, data]
                    # TODO: data needs to be stripped from: {" ", content:, pcre:}
                    rules.append(rule)
    return rules


def main():
    file_path = sys.argv[1]
    patterns = {'content': EXACT_MATCH_RULE_PATTERN,
                'pcre': REGEX_RULE_PATTERN}
    rules = parse_file(file_path, patterns)
    # TODO: split all 'pcre' rules to exact match (=ignore special regex characters)
    # TODO: discuss how to store content of type: |00 00 00 28 72 24|
    # TODO: save all findings to a file


if __name__ == "__main__":
    # Change working directory to script's dir.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()



