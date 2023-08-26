"""
Exact Match Extractor

This module provides functions for extracting exact match sub-strings from Perl Compatible Regular Expressions (pcre).

Authors:
    Idan Baruch (idan-b@campus.technion.ac.il)
    Itai Benyamin (itai.b@campus.technion.ac.il)

Functions:
    - char_to_utf8(character):
        Converts a character to its UTF-8 representation.
    
    - utf8_to_raw(input: str) -> str:
        Converts a UTF-8 encoded string to its raw representation.
    
    - utf8_to_ascii(input: str) -> list:
        Converts a UTF-8 encoded string to its ASCII values.
    
    - substitute_pattern(match: re.Match):
        Replaces a regex pattern of type 'c{2}' with the exact match string (e.g., 'cc').
    
    - replace_special_metacharacters(regex_pattern: str) -> str:
        Replaces special meta-characters in a Perl Compatible Regular Expression (pcre) with their UTF-8 representation.
    
    - extract_exact_matches(regex_pattern: str) -> list:
        Returns a list of non-ambiguous exact match sub-strings within a given pcre pattern.
    
    - run(pcre_string: str, flag='raw') -> list:
        Runs the Exact Match Extractor functions on a snort rule (pcre string) and returns a list of extracted sub-strings.
"""

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


def char_to_utf8(character):
    """
    Converts a character to its UTF-8 representation.
    :param character: A character.
    :return: A string representing the UTF-8 representation of the input character.
    """

    # Convert the character to its UTF-8 representation
    code_point = ord(character)
    utf8_hex = "\\x{:02X}".format(code_point)
    
    return utf8_hex  


def utf8_to_raw(input: str) -> str:
    """
    Converts a UTF-8 encoded string to its raw representation.

    :param input: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    # Convert the input string to raw representation
    raw_string = input
    
    if ('\\' not in input):
        raw_string = bytes(input, 'utf-8').decode('unicode-escape')

    return raw_string


def utf8_to_ascii(input: str) -> list:
    """
    Converts a UTF-8 encoded string to its ASCII values.
    
    :param input: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    
    # Get ASCII values of each character in the input string
    ascii_values = [ord(char) for char in input]

    return ascii_values


def substitute_pattern(match: re.Match):
    """
    Given a regex pattern of type: c{2} where 'c' is any character and '2' is any integer,
    the function replaces it with the exact match string (here for example it will be 'cc').

    :param match: A matched pattern from a regular expression containing capturing groups for character ('char')
        and positive integer ('num'), for example in c{2}: char = 'c', num = 2.

    :return: A string resulting from repeating the captured character 'char' a number of times specified by the captured
        integer 'num', for example in c{2} the return string will be 'cc'.
    """
    
    # Get the character and number of repetitions from the match
    char, num = match.groups()
    
    return char * int(num)


def replace_special_metacharacters(regex_pattern: str) -> str:
    """
    Given a Perl Compatible Regular Expression (pcre), this function will replace all special meta-characters with
    their UTF-8 representation.
    
    :param regex_pattern: A Perl Compatible Regular Expression (pcre) raw string in the form of r''.
    
    :return: A string representing the pcre pattern with all special meta-characters replaced with their UTF-8
    representation.
    """
    
    meta_characters = ['.', '^', '$', '*', '+', '?', '{', '}', '[', ']', '(', ')', '|', '\\', '/']
    utf8_metha_characters = [char_to_utf8(char) for char in meta_characters]
    
    for i, char in enumerate(meta_characters):
        regex_pattern = regex_pattern.replace('\\' + char, utf8_metha_characters[i])
    
    return regex_pattern    

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
        r'\/.{0,3}$',                                   # Removes pcre flags after '/' closure (/ims)
        r'(\\)([AbBdDfnrsStvwWzZ])([\+\*])?(\?)?',      # all regex of type \c or \c+ where c is a unique metacharacter.
        r'(\/)(\^)?',                                   # Removes pcre opening flag ('\' of '^')
        r'(\.)([\+\*])?(\?)?',                          # all regex of type '.*?' or '.+?' where ? might appear or not.
        r'\(\?:',                                       # all regex of type '(?:' indicating unwanted match groups.
        r'\(\?!',                                       # all regex of type '(?!' indicating negative lookahead.
        # r'(\(.*)\|(.*\))',              # INACTIVE:   # all regex of type a | b where a and b could be any symbols.
        r'\(',                            # OPTIONAL:   # Removing capturing groups '(' ')'
        r'\)',                            # OPTIONAL:   # -"-
        r'\|',                            # OPTIONAL:   # Removing Special char '|' for OR operation in regex
        r'\[.*?\]{\d+,?\d+?}',                          # all regex of type [abc]{2,50} where 2,50 are integers.
        r'\[.*?\]([\+\*])?(\?)?',                       # all regex of type [abc] where in [] could be any symbols.
        r'.{(\d+),(\d+)}',                              # all regex of type .{2,50} where . is any symbol and 2,50 ints.
        r'.{(\d+),}',                                   # all regex of type .{2,} where . is any symbol and 2 is int.
        r'.{\d+}',                                      # all regex of type .{2} where . is any symbol and 2 is integer.
        r'\(.*?\)([\+\*])?(\?)?',                       # all regex of type (something)*? where */+/? might appear.
        r'\\x..\?',                       # OPTIONAL:   # all regex of type \x..? where '.' could be any symbol.
        r'[^\\]\?',                                     # all regex of type c? where c is any symbol that isn't '\'.
    ]
    
    # Replaces all "canceled" ('\'+) special meta-characters with their UTF-8 representation.
    regex_pattern = replace_special_metacharacters(regex_pattern)
    
    # Replaces the unique *exact match* pattern (\w){(\d+)} for example: 'o{2}' in the actual string, for example: 'oo'
    regex_pattern = re.sub(r'(\w){(\d+)}', substitute_pattern, regex_pattern)

    # Replaces any non-exact match (ambiguous) patterns with an empty space ' '.
    for unwanted_pattern in unwanted_pattens:
        regex_pattern = re.sub(unwanted_pattern, ' ', regex_pattern)
    
    # Cleans the list of sub-strings from empty strings (''), and returns it.
    exact_matches_list = []
    for sub_string in regex_pattern.split(' '):
        if sub_string.strip():
            exact_matches_list.append(bytes(sub_string, 'utf-8').decode('unicode-escape'))
    
    return exact_matches_list


def run(pcre_string: str, flag='raw') -> list:
    """
    Runs the Exact Match Extractor functions on a snort rule (represented in a pcre string).
    :param pcre_string: A Perl Compatible Regular Expression (pcre) string.
    :param flag: A flag string indicating the wanted representation ('ascii' / 'raw') of the output list.
    :return: A list of extracted sub-strings of exact matches (non-ambiguous Regex patterns) as:
        flag == 'ascii': A list of arrays of integers representing the ASCII values of the sub-strings.
        flag == 'raw': A list of raw sub-strings of the exact matches.
    """
    
    matches = extract_exact_matches(pcre_string)
    if flag == 'ascii':
        return [utf8_to_ascii(sub_match) for sub_match in matches]
    elif flag == 'raw':
        return [utf8_to_raw(sub_match) for sub_match in matches]
    else:
        raise ValueError(f'Invalid flag: {flag}. flag must be either "ascii" or "raw".')
