import re
regex_string2 = r'/^GateCrasher\s+v\d+\x2E\d+\x2C\s+Server\s+On-Line\x2E\x2E\x2E/ims'
# the r'' indicated raw string in python (so that it won't replace \n with new line and so on).
# the '/' in the beginning and end represents the beginning and end of a regex expression in JavaScript / Perl.
# the 'ims' in the end specifying options:
#     'i' triggers case-insensitivity
#     'm' specifies multi-line mode
#     's' (IIRC) enables the dot-all option, meaning that the wildcard '.' includes newline characters.
# the '^' asserts, "The string must *begin* here, with no preceding characters..."
# the '$' at the end asserts, "...the string must *end* here, with no following characters."
# the '\s+' stands for one or more whitespace characters {' ', '\t', '\r\n', '\n'}
# the 'v' stands for the character 'v'.
# the '\d+' stands for one or more digit.
# the '\x2E' stands for the character '.' (= '\.' where the backslash '\' cancels the special wildcard effect of '.').
# the '\x2C' stands for the character ',' (= '\,')
# Wanted: 'GateCrasher', 'v', '.', ',', 'Server', 'On-Line...' (50%+ exact match, count base on num of chars?)
# Example: "GateCrasher v1.77,   Server On-Line..."


def utf8_to_raw_and_ascii(input_string):
    # Convert the input string to raw representation
    raw_string = bytes(input_string, 'utf-8').decode('unicode-escape')

    # Get ASCII values of each character in the input string
    ascii_values = [ord(char) for char in raw_string]

    return raw_string, ascii_values


def find_non_special_strings(regex_pattern):
    # Replace all '\_+' (except for \x), with a space).
    # TODO: implement a better way to exclude metacharacters and keep the \x__ UTF-8 coded characters for exact match.
    regex_pattern = re.sub(r'\\[a-wyzA-WYZ]\+?', ' ', regex_pattern)    # all \_ or \_+ without '\x'
    # Find all non-special character strings using \w+
    # For without \x__ expressions use: non_special_strings = re.findall(r'\b[\w-]+\b', regex_pattern)
    non_special_strings = re.findall(r'[\\\w-]+\b', regex_pattern)
    return non_special_strings


# Example regular expression string
# Fails for \r\n (that is represented by r'\\r\\n' or one of them would be [\\r\\n]
# TODO: Create a function that receives a Regex string and extracts an exact match list of strings (or arrays of ints of ASCII / UTF-8).
regex_string = r'/^GateCrasher\s+v\d+\x2E\d+\x2C\s+Server\s+On-Line\x2E\x2E\x2E/ims'
# TODO: Check for all pcre patterns, for example for the following one it removes the \r\n (is that wanted?).
regex_string2 = r'/^Connection:[^\\r\\n]*?X-F5-Auth/im'
# Note: Capturing groups are defined with parentheses

non_special_strings = find_non_special_strings(regex_string)
print("Sub-exact match expressions:")
raw_strings_list = []
for sub_match in non_special_strings:
    # print(sub_match)
    raw_string, ascii_values = utf8_to_raw_and_ascii(sub_match)
    raw_strings_list.append(raw_string)
    # print("Raw string:", raw_string)
    # print("ASCII values:", ascii_values)
print(raw_strings_list)


