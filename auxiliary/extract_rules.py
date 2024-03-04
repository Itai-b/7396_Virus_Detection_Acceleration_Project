#!/usr/bin/env python3
import os
import sys
import tarfile
import re

# Define regex patten of a wanted rule line
pattern = re.compile(r'sid:\d+')
bad_pattern = re.compile(r'alert \( gid:\d+')


# Function to check if dropped lines contain wanted signatures (content / pcre)
def contains_keywords(dropped_line):
    content_pattern = re.compile(r'content:".*?"')
    pcre_pattern = re.compile(r'pcre:".*?"')
    return content_pattern.search(dropped_line) or pcre_pattern.search(dropped_line)


# Check correctness of script call & input
if len(sys.argv) != 2:
    print(f'Usage: {sys.argv[0]} <path_to_zipped_folder_name.tar.gz>')
    sys.exit(1)

zipped_folder_path = sys.argv[1]
if not zipped_folder_path.endswith('.tar.gz'):
    print(f'Error: {sys.argv[1]} does not end with ".tar.gz"')
    sys.exit(1)

# Extract 'path/to/zipped_file' without .tar.gz ending
rules_dir = os.path.dirname(zipped_folder_path)
unzipped_file_name = os.path.splitext(os.path.basename(zipped_folder_path))[0]      # extract basename without .endings
unzipped_dir = os.path.join(rules_dir, unzipped_file_name)

# Create a new directory (if not exist) in 'path/to/zipped_file' to extract the zipped files to there
os.makedirs(unzipped_dir, exist_ok=True)

# Extract the folder from the zipped archive
with tarfile.open(zipped_folder_path, 'r:gz') as tar:
    tar.extractall(path=unzipped_dir, verbose=True)     # remove 'verbose=True' to avoid printing status of unzipping

# Create or empty the all.rules file
with open('all.rules', 'w'):
    pass

rules_count = 0
content_count = 0
pcre_count = 0
dropped_rules_count = 0
# Insert (to 'all.rules' file) rules that match the defined regex Pattern from all files ending with '.rules'
with open('all.rules', 'a') as all_rules:
    for root, dirs, files in os.walk(unzipped_dir):
        for file in files:
            if file.endswith('.rules'):
                with open(os.path.join(root, file), 'r') as rules_file:
                    for line in rules_file:
                        # searching for lines with rules that have 'sid' in them but not 'gid:...; sid:...' (not unique)
                        if pattern.search(line) and not bad_pattern.search(line):
                            all_rules.write(line)
                            rules_count += 1
                            if 'content' in line:
                                content_count += 1
                            if 'pcre' in line:
                                pcre_count += 1
                        elif contains_keywords(line):
                            print(f'Dropped line without Snort ID: {line.strip()}')
                            dropped_rules_count += 1

print(f'Finished appending all rules from: {unzipped_file_name} to: \'all.rules\'')
print(f'There were: {rules_count} rules')
print(f'            {content_count} rules with content signature(s)')
print(f'            {pcre_count} rules with pcre signature(s)')
print(f'{dropped_rules_count} rules were dropped (didn\'t have Snort ID)')
