import re
from config import config as config


def substitute_pipe_patterns(match: re.match):
    """
    Given a regex pattern of type: |00 01 02 ...| where '00', '01', ... symbolize characters,
    the function replaces it with the respective characters.

    :param match: A matched pattern from a regular expression containing capturing groups for pipes ('|')
        with sequences of numbers within, for example |00 72 38|

    :return: A string consisting of characters represented by the configured character encoding of the given hex values.
    """

    # Get only the hex sequence without the pipe symbols ('|')
    hex_sequence = match.groups(1)[0]
    # using ISO-8859-1 ('Latin-1') which, unlike utf-8 and utf-16, does *NOT* support multibyte characters.
    decoded_string = bytes.fromhex(hex_sequence.strip()).decode(config.CHARACTER_ENCODING)
    # TODO: python seems to not be able to print characters like \x00 or \x05 (but it still counts them)
    #   resulting an "empty" lists of content [] which have indeed characters inside and have length > 0.
    #   solve it by either find a better encoding format or by presenting all characters as HEX values.

    return decoded_string


def run(content_string: str) -> str:
    """
    Runs the Content Processor functions on a snort content rule.
    :param content_string: A snort content rule.
    :return: A list of chars of the given processed content rule.
    """
    content_string = content_string.lower()
    content_string = re.sub(r'(?:\|)(.*?)(?:\|)', substitute_pipe_patterns, content_string)
    return [[char for char in content_string]]
