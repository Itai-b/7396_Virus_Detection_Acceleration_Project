import re
# regex_string_example = r'/^GateCrasher\s+v\d+\x2E\d+\x2C\s+Server\s+On-Line\x2E\x2E\x2E/ims'
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


def utf8_to_raw(input:str) -> str:
    """
    Converts a UTF-8 encoded string to its raw representation.

    :param input: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    # Convert the input string to raw representation
    raw_string = bytes(input, 'utf-8').decode('unicode-escape')

    return raw_string

def utf8_to_ascii(input:str) -> list:
    """
    Converts a UTF-8 encoded string to its ASCII values.
    
    :param input: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    # Get ASCII values of each character in the input string
    ascii_values = [ord(char) for char in input]

    return ascii_values

new_var = (str, list)
def utf8_to_raw_and_ascii(input_string: str) -> new_var:
    """
    Converts a UTF-8 encoded string to its raw representation and provides ASCII values.

    :param input_string: The input string encoded in UTF-8.
    :return: A tuple containing two elements:
        [1] A string representing the raw representation of the input string.
        [2] A list of integers representing the ASCII values of each character in the raw representation.
    """
    # Convert the input string to raw representation
    raw_string = bytes(input_string, 'utf-8').decode('unicode-escape')

    # Get ASCII values of each character in the input string
    ascii_values = [ord(char) for char in raw_string]

    return raw_string, ascii_values


def substitute_pattern(match: re.Match):
    """
    Given a regex pattern of type: c{2} where 'c' is any character and '2' is any integer,
    the function replaces it with the exact match string (here for example it will be 'cc').

    :param match: A matched pattern from a regular expression containing capturing groups for character ('char')
        and positive integer ('num'), for example in c{2}: char = 'c', num = 2.

    :return: A string resulting from repeating the captured character 'char' a number of times specified by the captured
        integer 'num', for example in c{2} the return string will be 'cc'.
    """
    char, num = match.groups()
    return char * int(num)


def extract_exact_matches(regex_pattern: str) -> list:
    """
    Given a Perl Compatible Regular Expression (pcre), this function will return a list of sub-strings laying within
    the Regex, which aren't ambiguous and matched exactly (exact matches sub-strings).
    For example, for the pcre: r'/^GateCrasher\s+v\d+\x2E\d+x{2}/ims' it will return:
        ['GateCrasher','v','.','xx','ims'] where the last string is specifications (ims for case insensitivity).
    :param regex_pattern: A Perl Compatible Regular Expression (pcre) raw string in the form of r''.
    :return: A list of sub-strings of non-ambiguous exact matches within the given pcre patten.
    """
    unwanted_pattens = [
        r'(\/)(\^)?',
        r'\\[AbBdDfnrsStvwWzZ]\+?',
        r'(\\)([AbBdDfnrsStvwWzZ])([\+\*])?(\?)?',      # all regex of type \c or \c+ where c is a unique metacharacter.
        r'(\.)([\+\*])?(\?)?',                          # all regex of type '.*?' or '.+?' where ? might appear or not.
        r'(\(.*)\|(.*\))',                              # all regex of type a | b where a and b could be any symbols.
        r'\[.*?\]{.*?}',                                # all regex of type [abc]{2,50] where 2,50 are integers.
        r'\[.*?\]([\+\*])?(\?)?',                       # all regex of type [abc] where in [] could be any symbols.
        r'.{(\d+),(\d+)}',                              # all regex of type .{2,50} where . is any symbol and 2,50 ints.
        r'.{(\d+),}',                                   # all regex of type .{2,} where . is any symbol and 2 is int.
        r'.{.*?}',                                      # all regex of type .{2} where . is any symbol and 2 is integer.
        r'\(.*?\)([\+\*])?(\?)?',                       # all regex of type (something)*? where */+/? might appear.
        r'.\?',                                         # all regex of type c? where c is any symbol.
    ]
    # Replaces the unique *exact match* pattern (\w){(\d+)} for example: 'o{2}' in the actual string, for example: 'oo'
    regex_pattern = re.sub(r'(\w){(\d+)}', substitute_pattern, regex_pattern)

    # Replaces any non-exact match (ambiguous) patterns with an empty space ' '.
    for unwanted_pattern in unwanted_pattens:
        regex_pattern = re.sub(unwanted_pattern, ' ', regex_pattern)

    # Cleans the list of sub-strings from empty strings (''), and returns it.
    return [s for s in regex_pattern.split(' ') if s.strip()]


# TODO: Delete this part.
pcre_strings = [
    r'/^\bTest:p{3}[abcd-zA-Z\?]o{2,3}d{2,}\b',
    r'/^GateCrasher\s+v\d+\x2E\d+\x2C\s+Server\s+On-Line\x2E\x2E\x2E/ims',
    r'/^Connection:[^\\r\\n]*?X-F5-Auth/im',
    r'/\\x24\\x7b(jndi|[^\\x7d\\x80-\\xff]*?\\x24\\x7b[^\\x7d]*?\\x3a[^\\x7d]*?\\x7d)/i',
    r'/\\x24\\x7b.{0,200}\\x24\\x7b.{0,200}\\x3a[\\x27\\x22\\x2d\\x5c]*([jndi\\x7d\\x3a\\x2d]|\\x5cu00\[a-f0-9]{2}){1,4}[\\x22\\x27]?[\\x3a\\x7djndi]/i',
    r'/\\x24{(\\x24{(upper|lower):j}|j)(\\x24{(upper|lower):n}|n)(\\x24{(upper|lower):d}|d)(\\x24{(upper|lower):i}|i)(\\x24{(upper|lower)::}|:)/i',
    r'/vpn.*?(\\x2e|%(25)?2e){2}(\\x2f|%(25)?2f).*?vpns/i',
    r'/\\x24\\x7b[^\\x7d]*?com\\x2eopensymphony\\x2e(xwork2|webwork)\\x2e(Servlet)?ActionContext[^\\x7d]*?\\x28/i',
    r'/\\x24\\x7b[^\\x7d]*?sun\\x2emisc\\x2eUnsafe[^\\x7d]*?\\x28/i',
    r'/\\x24\\x7b[^\\x7d]*?atlassian\\x2e[^\\x7d]*?\\x28/i',
    r'/\\x24\\x7b[^\\x7d]*?javax?\\x2e[^\\x7d]*?\\x28/i',
    r'/\\x24%257b.{0,200}(%(25)?24|\\x24)(%(25)?7b|\\x7b).{0,200}(%(25)?3a|\\x3a)(%(25)?(27|2d|5c|22)|[\\x27\\x2d\\x5c\\x22])*([jndi\\x7d\\x3a\\x2d]|(%(25)?(7d|3a|2d))|(%(25)?5c|\\x5c)u00[a-f0-9]{2}){1,4}(%(25)?(22|27)|[\\x22\\x27])?(%(25)?(3a|7d)|[\\x3a\\x7djndi])/i',
    r'/\\x24\\x7b(jndi|[^\\x7d\\x80-\\xff]*?\\x24\\x7b[^\\x7d\\x80-\\xff]*?\\x3a[^\\x7d]*?\\x7d)/i',
    r'/Email=[^\\x3b\\r\\n]*?(\\x2f|%(25)?2f)autodiscover/i',
    r'/\\x2f[a-z0-9]{2,3}\\x2f/',
    r''
]

for i, pcre_string in enumerate(pcre_strings):
    matches = extract_exact_matches(pcre_string)
    print(f'Exact Matches for string_{i} "{pcre_string}":')
    # print(matches)
    normal_strings_list = []
    raw_strings_list = []
    ascii_values_list = []
    for sub_match in matches:
        # print(sub_match)
        normal_strings_list.append(sub_match.encode('utf-8').decode('unicode-escape'))
        # raw_string, ascii_values = utf8_to_raw_and_ascii(sub_match)
        # normal_string = raw_string.encode('utf-8').decode('unicode-escape')
        # raw_string, ascii_values = utf8_to_raw_and_ascii(raw_string)
        #normal_strings_list.append(normal_string)
        #raw_strings_list.append(raw_string)
        #ascii_values_list.append(ascii_values)

    print(normal_strings_list)
    print(raw_strings_list)
    print(ascii_values_list)
    print()

# TODO: figure out how to deal with ims flag.
# TODO: finish implementing run function and call it correctly in SnortRulesParser.
def run(pcre_string: str, flag='ascii') -> list:
    """
    Runs the Exact Match Extractor functions on a snort rule (represented in a pcre string).
    :param pcre_string: A Perl Compatible Regular Expression (pcre) string.
    :param flag: A flag string indicating the wanted representation ('ascii' / 'str') of the output list.
    :return: A list of extracted sub-strings of exact matches (non-ambiguous Regex patterns) as:
        flag == 'ascii': A list of arrays of integers representing the ASCII values of the sub-strings.
        flag == 'raw': A list of raw sub-strings of the exact matches.
    """
    matches = extract_exact_matches(pcre_string)
    if flag == 'ascii':
        return [utf8_to_ascii(sub_match) for sub_match in matches]
    elif flag == 'raw':
        return [utf8_to_raw(sub_match) for sub_match in matches]
    # TODO: fix the issue with utf8_to_raw function 
    else:
        raise ValueError(f'Invalid flag: {flag}. flag must be either "ascii" or "str".')
