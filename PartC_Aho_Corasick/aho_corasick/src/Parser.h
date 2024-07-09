#ifndef _PARSER_H
#define _PARSER_H

#include "ExactMatches.h"
#include "Statistics.h"
#include "bstring.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

std::size_t addBstring(const std::string& hexString, std::vector<bstring>& bstrings) {
    bstring bstring;
    hexToBstring(hexString, bstring);
    bstrings.push_back(bstring);
    return bstring.length();
}

/// <summary>
/// Convert the ExactMatches to std::basic_string<char>.
/// </summary>
/// <param name="exact_matches">Element of class ExactMatches, which is a vector of ExactMatch (rules,type,string) for each exact match extracted from the snort rules' signatures</param>
/// <param name="bstrings">An empty vector in which the basic_strings will be stored</param>
std::size_t toBstring(const ExactMatches& exact_matches, std::vector<bstring>& bstrings) {
    std::size_t max_length = 0;
    for (auto it = exact_matches.exact_matches->begin(); it != exact_matches.exact_matches->end(); ++it) {
        std::string hexString = (*it)->getExactMatch();
        //std::set<int> rules = (*it)->getRulesNumbers();
        //total_unique_rules.insert(rules.begin(), rules.end());
        //bstrings.push_back(addBstring(hexString));
        std::size_t length = addBstring(hexString, bstrings);
        if (length > max_length) {
            max_length = length;
        }
    }
    return max_length;
}


/// <summary>
/// Convert a SearchResults to std::basic_string<char>.
/// </summary>
/// <param name="search_results">A vector of SearchResults (search_key,
/// _sids,sids_hit) for each pattern to search in the built aho corasick TRIE</param>
/// <param name="search_strings">An empty vector in which the basic_strings will be stored</param>
void toBstring(const std::vector<SearchResults>* search_results, std::vector<bstring>& bstrings) {
    if (search_results == nullptr) {
        return;
    }
    for (SearchResults search_item : *search_results) {
        std::string hexString = search_item.search_key;
        addBstring(hexString, bstrings);
    }
}

/**
 * Parse a line in 'exact_matches_hex.json' to extract items of class ExactMatch.
 *
 * @param line string representing a line in the parsed .json file
 * @param exact_matches reference to a member of class ExactMaches,
 * that the results of the parsing will be inserted to
 */
void parseLine(std::string line, ExactMatches& exact_matches) {
    nlohmann::json jsonObj = nlohmann::json::parse(line);

    // Accessing elements in the JSON object
    std::vector<std::string> exact_match_hex = jsonObj["exact_match_hex"];
    std::vector<int> non_unique_rules = jsonObj["rules"];
    std::set<int> rules(non_unique_rules.begin(), non_unique_rules.end());

    std::string match = "0x";

    int j = 0;
    for (const auto& hex : exact_match_hex) {
        std::string value_to_concat = "";
        if ((hex.substr(0, 2) == "0x")) {
            value_to_concat = hex.substr(2);
        }
        match = match + value_to_concat;
        ++j;
    }
    //insert exact_match to ExactMatches.
    exact_matches.insert(ExactMatch(rules, match));
}

 /**
  * Parse a line in 'parta_data.json' to extract items of class ExactMatch.
  *
  * @param line string representing a line in the parsed .json file
  * @param exact_matches reference to a member of class ExactMaches,
  * that the results of the parsing will be inserted to
  */
void parseFile(std::string file_path, ExactMatches& exact_matches) {
    try {
        std::ifstream input_file(file_path);

        if (!input_file.is_open()) {
            throw std::runtime_error("Error opening the file.");
        }

        std::string line;
        while(std::getline(input_file, line)) {
            parseLine(line, exact_matches);
        }

        input_file.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


/// <summary>
/// Parse a JSON test file containing hexStrings of signatures patterns to search.
/// </summary>
/// <param name="file_path">Path to JSON file containing the search hexStrings</param>
/// <param name="res">A vector containing SearchResults element for each search pattern</param>
void parseFile(std::string file_path, std::vector<SearchResults>& res) {
    std::ifstream inputFile(file_path);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open .json file" << std::endl;
        exit(1);
    }

    nlohmann::json jsonObj;
    inputFile >> jsonObj;

    inputFile.close();

    for (const auto& item : jsonObj) {
        SearchResults search_item;
        search_item.original_sids = item["sids"].get<std::vector<int>>();
        std::string tmp_string = item["hex_string_example"];

        // Process the string from a form of {FF FF FF FF ...} -> {0xFFFFFFFF...}
        std::ostringstream oss;
        oss << "0x";
        for (std::size_t i = 0; i < tmp_string.size(); ++i) {
            if (tmp_string[i] != ' ') {
                oss << tmp_string[i];
            }
        }

        search_item.search_key = oss.str();
        res.push_back(search_item);
    }
}


#endif // !_PARSER_H
