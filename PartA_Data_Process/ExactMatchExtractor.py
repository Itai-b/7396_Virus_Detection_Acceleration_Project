"""
Exact Match Extractor

This module provides functions for extracting exact match exactmatches from Perl Compatible Regular Expressions (pcre).

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
    
    - substitute_signature(match: re.Match):
        Replaces a regex signature of type 'c{2}' with the exact match string (e.g., 'cc').
    
    - replace_special_metacharacters(regex_signature: str) -> str:
        Replaces special meta-characters in a Perl Compatible Regular Expression (pcre) with their UTF-8 representation.
    
    - extract_exact_matches(regex_signature: str) -> list:
        Returns a list of non-ambiguous exact match exact-matches within a given pcre signature.
    
    - run(pcre_string: str, flag='raw') -> list:
        Runs the Exact Match Extractor functions on a pcre string and returns a list of extracted exact-matches.
"""

import re
from config import config

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


def utf8_to_raw(input_string: str) -> str:
    """
    Converts a UTF-8 encoded string to its raw representation.

    :param input_string: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    # Convert the input string to raw representation
    raw_string = input_string
    
    if '\\' not in input_string:
        raw_string = bytes(input_string, 'utf-8').decode('unicode-escape') #'unicode-escape'

    return raw_string


def utf8_to_ascii(input_string: str) -> list:
    """
    Converts a UTF-8 encoded string to its ASCII values.
    
    :param input_string: The input string encoded in UTF-8.
    :return: A string representing the raw representation of the input string.
    """
    
    # Get ASCII values of each character in the input string
    ascii_values = [ord(char) for char in input_string]
    return ascii_values


def substitute_signature(match: re.match):
    """
    Given a regex signature of type: c{2} where 'c' is any character and '2' is any integer,
    the function replaces it with the exact match string (here for example it will be 'cc').

    :param match: A matched signature from a regular expression containing capturing groups for character ('char')
        and positive integer ('num'), for example in c{2}: char = 'c', num = 2.

    :return: A string resulting from repeating the captured character 'char' a number of times specified by the captured
        integer 'num', for example in c{2} the return string will be 'cc'.
    """
    
    # Get the character and number of repetitions from the match
    char, num = match.groups()
    
    return char * int(num)


def replace_special_metacharacters(regex_signature: str) -> str:
    """
    Given a Perl Compatible Regular Expression (pcre), this function will replace all special meta-characters with
    their UTF-8 representation.
    
    :param regex_signature: A Perl Compatible Regular Expression (pcre) raw string in the form of r''.
    
    :return: A string representing the pcre signature with all special meta-characters replaced with their UTF-8
    representation.
    """

    utf8_meta_characters = [char_to_utf8(char) for char in config.meta_characters]

    for i, char in enumerate(config.meta_characters):
        regex_signature = regex_signature.replace('\\' + char, utf8_meta_characters[i])
    
    return regex_signature    


def extract_exact_matches(regex_signature: str) -> list:
    """
    Given a Perl Compatible Regular Expression (pcre), this function will return a list of exactmatches laying within
    the Regex, which aren't ambiguous and matched exactly (exact matches exactmatches).
    
    For example, for the pcre: r'/^GateCrasher\s+v\d+\x2E\d+x{2}/ims' it will return:
        ['GateCrasher','v','.','xx','ims'] where the last string is specifications (ims for case insensitivity).
    
        :param regex_signature: A Perl Compatible Regular Expression (pcre) raw string in the form of r''.
    
        :return: A list of exactmatches of non-ambiguous exact matches within the given pcre patten.
    """
    
    # Replaces all "canceled" ('\'+) special meta-characters with their UTF-8 representation.
    regex_signature = replace_special_metacharacters(regex_signature)
    
    # Replaces the unique *exact match* signature (\w){(\d+)} for example: 'o{2}' in the actual string, for example: 'oo'
    regex_signature = re.sub(r'(\w){(\d+)}', substitute_signature, regex_signature)

    # Replaces any non-exact match (ambiguous) signatures with an empty space ' '.
    for unwanted_signature in config.unwanted_pattens:
        regex_signature = re.sub(unwanted_signature, ' ', regex_signature)
    
    # Cleans the list of exactmatches from empty strings (''), and returns it.
    exact_matches_list = []
    for exact_match in regex_signature.lower().split(' '):
        if exact_match.strip():
            exact_matches_list.append(bytes(exact_match, 'utf-8').decode('unicode-escape')) #'unicode-escape'

    return exact_matches_list


def run(pcre_string: str, flag=config.RESULTS_FORM) -> list:
    """
    Runs the Exact Match Extractor functions on a snort rule (represented in a pcre string).
    :param pcre_string: A Perl Compatible Regular Expression (pcre) string.
    :param flag: A flag string indicating the wanted representation ('ascii' / 'raw') of the output list.
    :return: A list of extracted exactmatches of exact matches (non-ambiguous Regex signatures) as:
        flag == 'ascii': A list of arrays of integers representing the ASCII values of the exactmatches.
        flag == 'raw': A list of raw exactmatches of the exact matches.
    """
    
    matches = extract_exact_matches(pcre_string)
    if flag == 'ascii':
        return [utf8_to_ascii(sub_match) for sub_match in matches]
    elif flag == 'raw':
        return [utf8_to_raw(sub_match) for sub_match in matches]
    elif flag == 'char':
        return [[char for char in exactmatch] for exactmatch in matches]
    else:
        raise ValueError(f'Invalid flag: {flag}. flag must be either "ascii" or "raw".')
