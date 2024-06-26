import os
import argparse
import re
import logging
import time
import json

import ExactMatchExtractor as ExactMatchExtractor
import ContentProcessor as ContentProcessor
import ResultsAnalysis as ResultsAnalysis
from datetime import datetime

logger = logging.getLogger('SnortRulesParser')

CONTENT_SIGNATURE = r'(?:content:")(.*?)(?:"[,;])'  # exact match rules
PCRE_SIGNATURE = r'(?:pcre:")(.*?)(?:";)'           # perl compatible regular expression rules
RULE_NUMBER = r'(?:sid:)(\d+)(?:;)'                 # rule sid

def add_data_by_signature(signature, signature_type, exact_matches, exact_matches_hex, rule, data_by_signature):
    for line in data_by_signature:
        if (signature == line["signature"]):
            if rule not in line["rules"]:
                line["rules"].append(rule)
            if signature_type not in line["signature_type"]:
                line["signature_type"].append(signature_type)
            return
    # If the signature is not found in the data_by_signature list, add it.
    if signature_type == 'pcre':
        ResultsAnalysis.total_pcre += 1
    else:  # signature_type == 'content':
        ResultsAnalysis.total_content += 1
    data_by_signature.append({"signature": signature, "signature_type": signature_type, "exact_matches": exact_matches, "exact_matches_hex": exact_matches_hex, "rules": [rule]})

def add_data_by_exactmatch(exact_matches, exact_matches_hex, rule, signature_type, data_by_exactmatch):
    if exact_matches == [] and exact_matches_hex == []:
        for line in data_by_exactmatch:
            if line["exact_match"] == [] and line["exact_match_hex"] == []:
                if rule not in line["rules"]:
                    line["rules"].append(rule)
                if signature_type not in line["signature_type"]:
                    line["signature_type"].append(signature_type)
                return
        data_by_exactmatch.append({"exact_match": [], "exact_match_hex": [], "signature_type": [signature_type], "rules": [rule]})
        return
    
    for i, exact_match in enumerate(exact_matches):
        found = False
        for line in data_by_exactmatch:
            if (exact_match == line["exact_match"]):
                found = True
                if rule not in line["rules"]:
                    line["rules"].append(rule)
                if signature_type not in line["signature_type"]:
                    line["signature_type"].append(signature_type)
                break
        if not found:
            data_by_exactmatch.append({"exact_match": exact_match, "exact_match_hex": exact_matches_hex[i], "signature_type": [signature_type], "rules": [rule]})
    
def save_data_as_json(data_by_signature, data_by_exactmatch, save_path):
    """
        An auxiliary function used to save part_a data to a json file.
    """
    json_path = os.path.join(save_path, 'parta_data_by_signature.json')
    with open(json_path, 'w') as file:
        for line in data_by_signature:
            json.dump(line, file, indent=None)
            file.write('\n')
            
    json_path = os.path.join(save_path, 'parta_data_by_exactmatch.json')
    with open(json_path, 'w') as file:
        for line in data_by_exactmatch:
            json.dump(line, file, indent=None)
            file.write('\n')
             
    logger.info(f"Saved the partA data as .json file under the path {save_path}")

def parse_file(file_name, signatures, rules_sid):
    
    with open(file_name, 'r') as file:
        rules = file.readlines()
    
    data_by_signature = []
    data_by_exactmatch = []
    
    ResultsAnalysis.total_rules = len(rules)
    
    for rule in rules:
        rule_sid = int(re.search(RULE_NUMBER, rule).group(1))
        rules_sid.append(rule_sid)
        for signature_type, signature in signatures.items():
            matches = re.finditer(signature, rule)
            if matches:
                for match in matches:                       
                    signature = match.group(1)

                    exact_matches = []
                    if signature_type == 'pcre':
                        exact_matches = ExactMatchExtractor.run(signature, 'char')
                    else:   # signature_type == 'content':
                        exact_matches = ContentProcessor.run(signature.lower())
                    
                    exact_matches_hex = translate_exact_matches_to_hex(exact_matches)
                    #add_exact_matches_to_data(signature, exact_matches, exact_matches_hex, data_by_signature)
                    add_data_by_signature(signature, signature_type, exact_matches, exact_matches_hex, rule_sid, data_by_signature)
                    add_data_by_exactmatch(exact_matches, exact_matches_hex, rule_sid, signature_type, data_by_exactmatch)
                    ResultsAnalysis.total_exactmatches += len(exact_matches)

    return data_by_signature, data_by_exactmatch

def translate_exact_matches_to_hex(exact_matches):
    """
        An auxiliary function used to translate exact_matche to hex.
    """
    
    exact_matches_hex = []
    for exact_match in exact_matches:
        hex_list = []
        for i, char_list in enumerate(exact_match):
            for char in char_list:
                hex_value = (hex(ord(char)))
                if len(hex_value) < 4:
                    hex_list.append('0x' + '0' + hex_value[-1])
                else:
                    hex_list.append(hex_value)
        exact_matches_hex.append(hex_list)
    return exact_matches_hex            


def main():
    """
        Usage (in Terminal): python SnortRuleParser.py snort3-community.rules
    """
    parser = argparse.ArgumentParser(description="Snort Rule Set Extractor")
    parser.add_argument('-r', '--file_name', type=str, default='snort3-community.rules', help='The path for the snort.rules file to parse. Default is snort3-community')
    parser.add_argument('-p', '--path', type=str, default=os.getcwd(), help='The path where to save all the files')
    parser.add_argument('-d', '--debug', action='store_true', help='Prints debug information about the script\'s execution.')
    parser.add_argument('-j', '--json', action='store_true', help='Saves the exact matches as a .json file.')
    
    args = parser.parse_args()    

    abs_save_path = os.path.abspath(args.path)
    logger.info(f'The path where the files will be saved is {abs_save_path}.')

    file_path = ""
    if args.file_name:
        file_path = args.file_name    
    else:
        # If no file name is given, the script will expect that the file is in the same directory as the script.
        file_path = 'snort3-community.rules' 
    
    logging.basicConfig(level=logging.INFO)
    # Create a FileHandler to save log messages to a file
    log_file_handler = logging.FileHandler(os.path.join(abs_save_path,'SnortRuleParser.log'))
    # Clear the log file from previous runs
    with open(os.path.join(abs_save_path,'SnortRuleParser.log'), 'w') as file:
        pass
    
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
    
    # Create a formatter to specify the format of log messages
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    log_file_handler.setFormatter(formatter)
    logger.addHandler(log_file_handler)    

    signatures_type = {'content': CONTENT_SIGNATURE,
                'pcre': PCRE_SIGNATURE}
    
    logger.info(f'Started parsing.')
    start_time = time.time()                          

    print(f'file path: {file_path}')
    data_by_signature, data_by_exactmatch = parse_file(file_path, signatures_type, ResultsAnalysis.rules_sid)
    ResultsAnalysis.unique_exactmatches = len(data_by_exactmatch)    

    ResultsAnalysis.main(data_by_signature, data_by_exactmatch, abs_save_path, logger)
    
    if args.json:
        save_data_as_json(data_by_signature, data_by_exactmatch, abs_save_path)
        
    logger.info(f'Finished parsing the file.\n')
    end_time = time.time()
    ResultsAnalysis.log_info(start_time, end_time, logger)

if __name__ == "__main__":
    """
        Change working directory to script's directory.
    """
    # +md  Change working directory to script's directory.
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    main()
