from iblt import IBLT
from matplotlib import pyplot as plt
import os
import argparse
import json

class Data:
    def __init__(self, m, rules):
        self.m = m
        self.rules_set = rules
        self.success_rate = 0
        self.iblts_total_size = 0

    def calculate_success_rate(self):
        total_success_rate = 0
        for rules in self.rules_set:
            iblt = IBLT(self.m, K, KEYSIZE, VALUESIZE)
            for rule in rules:
                iblt.insert(str(rule), str(rule))
            size = iblt.get_serialized_size()
            self.iblts_total_size += size
            entries = iblt.list_entries()
            if entries[0] == IBLT.RESULT_LIST_ENTRIES_INCOMPLETE:
                entries_size = len(entries[1])
                success_rate = (entries_size/len(rules))
                total_success_rate += success_rate
            else:
                total_success_rate += 1
        self.success_rate = total_success_rate/len(self.rules_set)
    
    def calculate_success_rate_optimal(self):
        total_success_rate = 0
        for rules in self.rules_set:
            iblt_size = int(2*len(rules))
            iblt = IBLT(iblt_size, K, KEYSIZE, VALUESIZE)
            for rule in rules:
                iblt.insert(str(rule), str(rule))
            size = iblt.get_serialized_size()
            self.iblts_total_size += size
            entries = iblt.list_entries()
            if entries[0] == IBLT.RESULT_LIST_ENTRIES_INCOMPLETE:
                entries_size = len(entries[1])
                success_rate = (entries_size/len(rules))
                total_success_rate += success_rate
            else:
                total_success_rate += 1
        self.success_rate = total_success_rate/len(self.rules_set)

def check_json_file_type(file_path):
    with open(file_path, 'r') as file:
        # Skip leading whitespace
        while True:
            char = file.read(1)
            if not char.isspace():
                break

        if char == '[':
            return 'Array of objects'
        elif char == '{':
            return 'Series of objects'
        else:
            return 'Invalid JSON'

def parse_json(json_path):
    # Read the .json file and parse it to a list of rules
    # Check if .json file is an array of objects or a series of objects
    file_type = check_json_file_type(json_path)
    
    if file_type == 'Invalid JSON':
        print("Error: Invalid JSON file.")
        exit(1)
    rules = []
    with open(json_path, 'r') as file:
        if file_type == 'Series of objects':    
            for line in file:
                data = json.loads(line)
                rules.append(data['rules'])
        else: # Array of objects
            data = json.load(file)
            for obj in data:
                rules.append(obj['rules'])
    return rules

NAME = ""
SAVE_PATH = os.path.join('..', 'Data', 'PartD_Data', 'IBLT_JSONs')
VALUESIZE = 10
KEYSIZE = 10
K = 6
M = [2, 4, 8, 16, 32, 64, 128, 256, 512]
def main():
    global NAME, SAVE_PATH
    # get from args the path to .json file
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--path", help="Path to .json file", required=True)
    parser.add_argument("-n", "--name", help="Name of the file to save", required=True)
    args = parser.parse_args()

    json_path = args.path

    # Check if the path is valid and points to a .json file
    if not os.path.isfile(json_path) or not json_path.endswith('.json'):
        print("Error: Invalid path or file format. Please provide a valid .json file.")
        return 1
    
    # Check the file name provided to save the results is valid
    if not args.name:
        print("Error: Please provide a valid file name to save the results.")
        return 1

    NAME = args.name

    results = []
    # Create an instance of IBLT
    rules = parse_json(json_path)

    # optimal check
    data = Data(0, rules)
    data.calculate_success_rate_optimal()
    print('Optimal Check')
    print('list_enteries success rate: ', data.success_rate)
    print('total size in Bytes: ', data.iblts_total_size)

    for m in M:
        data = Data(m, rules)
        data.calculate_success_rate()
        print('Cells number: ', m)
        print('list_enteries success rate: ', data.success_rate)
        #print('total size in Bytes: ', data.iblts_total_size)
        results.append({'m': m, 'success_rate': data.success_rate, 'total_size': data.iblts_total_size})

    # Save the results to a .json file
    # Create the directory if it doesn't exist using mkdir -p
    if not os.path.exists(SAVE_PATH):
        os.makedirs(SAVE_PATH)
        
    file_path = os.path.join(SAVE_PATH, f'iblt_theoretical_checks_{NAME}.json')
    with open(file_path, 'w') as file:
        for result in results:
            file.write(json.dumps(result) + '\n')

    return 0

if __name__ == "__main__":
    #set working directory to script directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    exit_code = main()
    exit(exit_code)