import re


def extract_sub_matches(regex_string):
    sub_matches = []

    # Find all capturing groups in the regular expression
    capturing_groups = re.findall(r'\((?!\?:)(.*?)\)', regex_string)

    # Filter out empty strings and duplicates
    sub_matches = list(filter(None, capturing_groups))

    return sub_matches

# Example regular expression string
regex_string = r'/^GateCrasher\s+v\d+\x2E\d+\x2C\s+Server\s+On-Line\x2E\x2E\x2E/ims'

sub_matches = extract_sub_matches(regex_string)
print("Sub-exact match expressions:")
for sub_match in sub_matches:
    print(sub_match)