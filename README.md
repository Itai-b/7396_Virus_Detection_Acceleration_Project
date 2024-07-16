
# Accelerating Virus Detection in Web Traffic: Exploring Efficient Implementation Strategies

Nowadays, numerous online algorithms are available for identifying virus-infected code snippets. An inherent challenge lies in the considerable computational and associated with these algorithms and their time-consuming regular expressions (regex) matching. As a solution for this challenge, an early classification stage is introduced.

Our objective is the feasibility investigation of a fast and robust preliminary traffic classifier based on exact string matching. This feasibility check is implemented in this repo end-to-end.

The first part of the repo introduces a generic fully automated script to extract non-ambiguous keywords ("exact matches") out of any given Snort ruleset. In the second and third parts, data structures to store the exact matches parsed, namely cuckoo hash tables and Aho-Corasick automata, are evaluated respectively in end-to-end tests. The last part of this repo investigates the possibility to lower the space cosumption of the entire data structure using Invertible Bloom Lookup Tables (IBLT).

## Project's Stages

1) Data Process: parse content and pcre rules from [Snort's ruleset](https://www.snort.org/downloads#rules) to build a map of exact-matches and the rules assigned to them. 

2) Insert the processed data into a Hashtable while optimizing the table size and its utilization according to the length of each substring and the gap between two substrings parsed. (this repo uses [libcuckoo](https://github.com/efficient/libcuckoo))

3) Insert the processed data into Aho-Corasick. (this repo uses cjgdev's [aho_corasick](https://github.com/cjgdev/aho_corasick) implementation)

4) Evaluate the best performing candidates, for the data-structure of the classifier, from each category.
This is based on an End-to-End testing.

5) IBLT Check - investigation for more space efficient option. (this repo uses jasperborgstup's [iblt](https://github.com/jesperborgstrup/Py-IBLT) implementation) 

## Usage
The project can be run using an automated script named `run_project.sh`.
```bash
./run_project.sh -p {snort.rules} -t {test_file}
```
- `snort.rules` is the path to the snort.rules file you wish to use. If not specified the script will use `Auxiliary/snort3-commuinty.rules`
- `test_file` is the path to the desired test_file you wish to use. If not specified the script will use `Auxiliary/end_to_end_test.json` (you can check the deafult file to see how to construct a test file).

Post-running, a directory named `Data` will be created with all the relevant files and statistics.

### Disclaimer:
* This script was tested on `wsl` using Ubuntu 20.04.6 distribution.
* Try using `dos2unix` and `chmod +x` if you are having difficulties running the script.

## Authors
- [Idan Baruch](https://github.com/idanbaru)  - Idan-b@campus.technion.ac.il
- [Itai Benyamin](https://github.com/Itai-b) - Itai.b@campus.technion.ac.il
- This project is under the supervision of Alon Rashelbach in the ACSL lab at the Technion.
