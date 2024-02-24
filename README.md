
# Accelerating Virus Detection in Web Traffic: Exploring Efficient Implementation Strategies

Nowadays, numerous online algorithms are available for identifying virus-infected code snippets. An inherent challenge lies in the considerable computational and memory expenses associated with these algorithms. Our objective revolves around investigating the feasibility of extracting virus code exact-match substrings, incorporating them into a hashtable, and subsequently employing dedicated hardware to execute an algorithm for an initial virus search. This approach aims to enhance efficiency and dependability in virus detection.

## Project's Stages

1) Data Process: extract content and pcre rules from [Snort's ruleset](https://www.snort.org/downloads#rules) to build an exact-match sub-signatures ruleset 

2) Insert the processed data into a Hashtable while optimizing the table size and its utilization according to the length of each substring and the gap between two substrings. (we have used [libcuckoo](https://github.com/efficient/libcuckoo))

3) Given that stage 2 results are not satisfying enough, the data will be processed by the Aho-Corasick algorithm. 

4) Given that stage 3 results are not satisfying enough, the data will be processed by a dedicated algorithm that was developed in the ACSL lab.

## Usage
The project can be run using an automated script named `run_project.sh`.
```bash
./run_project.sh {snort.rules}
```
Were `snort.rules` is the path to the snort.rules file you wish to use. If not specified the script will use `Auxillary/snort3-commuinty.rules`

Postrunning a directory named `Data` will be created will the relevant files and statistics.

### Disclaimer:
* This script was tested on `wsl` using Ubuntu 20.04.6 distribution.
* Try using `dos2unix` if the script fails to run.

## Authors

- [Idan Baruch](https://github.com/idanbaru)  - Idan-b@campus.technion.ac.il
- [Itai Benyamin](https://github.com/Itai-b) - Itai.b@campus.technion.ac.il
- This project is under the supervision of Alon Rashelbach in the ACSL lab at the Technion.
