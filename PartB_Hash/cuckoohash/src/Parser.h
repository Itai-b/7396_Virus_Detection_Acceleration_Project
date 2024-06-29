#ifndef _PARSER_H
#define _PARSER_H

#include "ExactMatches.h"
#include "Substring.h"
#include "Statistics.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>


/// <summary>
/// Parse the ExactMatches and extract Substrings of L bytes with parsing of G bytes jump gap per substring.
/// </summary>
/// <typeparam name="T">Type of the unsigned int which will represent the substring {uint32_t, uint64_t, ...}</typeparam>
/// <param name="exact_matches">Element of class ExactMatches, which is a vector of ExactMatch (rules,type,string) for each exact match extracted from the snort rules' signatures</param>
/// <param name="substrings">The set of Substrings in which the results will be stored</param>
template<typename T, std::size_t G = SUBSTRING_DEFAULT_GAP>
std::size_t parseExactMatches(const ExactMatches& exact_matches, std::vector<Substring<T>>& substrings, SubstringLogger& log) {
    std::set<int> total_unique_rules;
    
    // Extract substrings for each exact match string and store it into the substrings vector
    for (auto it = exact_matches.exact_matches->begin(); it != exact_matches.exact_matches->end(); ++it) {
        std::string hexString = (*it)->getExactMatch();
        std::set<int> rules = (*it)->getRulesNumbers();
        total_unique_rules.insert(rules.begin(), rules.end());
        Substring<T>::extractSubstrings(hexString, substrings, rules, G);
    }

    // Parse the unique* substrings to log them into the substrings_log.json
    // (*see extractSubstrings implementation)
    for (Substring<T> substring : substrings) {
        log.logSubstringData({
            static_cast<std::size_t>(substring.getSubstring()),
            substring.toStringHex(),
            substring.str(),        // for full representation use: "substring.toStringFull()," instead
            static_cast<std::size_t>(substring.getNumOfDups()),
            std::set<int>(substring.rules->begin(), substring.rules->end())
        });
    }
    std::size_t num_of_unique_rules = total_unique_rules.size();
    return num_of_unique_rules;
}

template<typename T>
std::size_t getTotalNumOfDups(const std::vector<Substring<T>>& substrings) {
    std::size_t num_of_duplicates = 0;
    for (Substring<T> substring : substrings) {
        num_of_duplicates += static_cast<std::size_t>(substring.getNumOfDups());
    }
    return num_of_duplicates;
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

    
    std::vector<std::string> exact_match_str = jsonObj["exact_match"];
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
    exact_matches.insert(ExactMatch(rules, match));
}

 /**
  * Parse 'parta_data.json' to extract items of class ExactMatch.
  *
  * @param file_path string representing a .json file to be parsed
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
    std::cout << "file path:" << file_path << std::endl;
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
