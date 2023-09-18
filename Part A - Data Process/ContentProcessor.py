import re


def substitute_pipe_patterns(match: re.match):
    """
    Given a regex pattern of type: |00 01 02 ...| where '00', '01', ... symbolize characters,
    the function replaces it with the respective characters.

    :param match: A matched pattern from a regular expression containing capturing groups for pipes ('|')
        with sequences of numbers within, for example |00 72 38|

    :return: A string resulting from
    """

    # Get only the hex sequence without the pipe symbols ('|')
    hex_sequence = match.groups(1)[0]
    print(hex_sequence)
    decoded_string = bytes.fromhex(hex_sequence.strip()).decode('utf-8')

    return decoded_string


def run(content_string: str) -> str:
    """
    Runs the Exact Match Extractor functions on a snort rule (represented in a pcre string).
    :param content_string: A snort content rule.
    :return:
    """
    content_string = content_string.lower()
    content_string = re.sub(r'B4', '00', content_string)
    content_string = re.sub(r'85', '00', content_string)
    content_string = re.sub(r'(?:\|)(.*?)(?:\|)', substitute_pipe_patterns, content_string)
    print(content_string)
    return content_string
