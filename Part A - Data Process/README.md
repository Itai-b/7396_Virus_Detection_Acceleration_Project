# Snort Rule Set Extractor

This repository contains three Python scripts: `SnortRuleParser.py`, `ExactMatchExtractor.py` and `ContentProccessor.py`, which are designed to extract rules and exact match sub-strings from Snort's rule set, respectively.

## SnortRuleParser.py

### Description
`SnortRuleParser.py` is a script that extracts rules or sub-strings (for exact match) from Snort's rule set. It supports extracting exact match rules and Perl compatible regular expression rules. The extracted rules can be saved as a JSON file or printed to the console.

### Usage
Run the script from the terminal using the following command: `python SnortRulesParser.py snort3-community.rules`

The script takes the following arguments:
- `snort3-community.rules`: the path to the Snort rule set file. 
If the file is not in the same directory as the script, the full path must be provided.

The script supports the following flags:
- `-json`: saves the extracted rules as a JSON file. Otherwise, if not specified, the rules are printed to the console.

## ExactMatchExtractor.py

### Description
`ExactMatchExtractor.py` is a custom module designed to extract exact match sub-strings from Perl Compatible Regular Expressions (pcre). It provides functions to convert characters to their UTF-8 representation, transform UTF-8 encoded strings to raw representation, and extract exact match sub-strings from pcre patterns.

### Functions
- `char_to_utf8(character)`: Converts a character to its UTF-8 representation.
- `utf8_to_raw(input: str) -> str`: Converts a UTF-8 encoded string to its raw representation.
- `utf8_to_ascii(input: str) -> list`: Converts a UTF-8 encoded string to its ASCII values.
- `substitute_pattern(match: re.Match)`: Replaces a regex pattern of type 'c{2}' with the exact match string.
- `replace_special_metacharacters(regex_pattern: str) -> str`: Replaces special meta-characters in a pcre with their UTF-8 representation.
- `extract_exact_matches(regex_pattern: str) -> list`: Returns a list of non-ambiguous exact match sub-strings within a pcre pattern.
- `run(pcre_string: str, flag='raw') -> list`: Runs the Exact Match Extractor functions on a pcre string.

## ContentProccessor.py

### Description
`ContentProccessor.py` is a custom module designed to process content modifiers in Snort rules. It provides functions to extract content modifiers from a Snort rule, and to extract exact match sub-strings from content modifiers.

### Functions
- `substitute_pipe_patterns (match: re.Match)`: Replaces a regex pattern of type 'c{2}' with the exact match string.
- `run(content: str) -> list`: Runs the Exact Match Extractor functions on a content modifier string.
 
## Authors
- [Idan Baruch](https://github.com/idanbaru)  - Idan-b@campus.technion.ac.il
- [Itai Benyamin](https://github.com/Itai-b) - Itai.b@campus.technion.ac.il
