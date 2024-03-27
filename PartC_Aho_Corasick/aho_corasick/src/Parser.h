#ifndef _PARSER_H
#define _PARSER_H

#include "ExactMatches.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

typedef std::basic_string<char> bstring;

bstring hexStringToBstring(const std::string& hexString) {
    // Remove the "0x" prefix if present
    std::string cleanedHexString = hexString.substr(0, 2) == "0x" ? hexString.substr(2) : hexString;

    size_t len = cleanedHexString.size();
    // Ensure the input string has an even number of characters
    if (len % 2 != 0) {
        throw std::invalid_argument("Hex string length must be even!");
    }

    // Convert each pair of hexadecimal characters to a byte
   bstring result;
    for (size_t i = 0; i < len; i += 2) {
        std::istringstream iss(cleanedHexString.substr(i, 2));
        int byteValue;
        iss >> std::hex >> byteValue;
        result.push_back(static_cast<char>(byteValue));
    }

    return result;
}

/// <summary>
/// Convert the ExactMatches to std::basic_string<char>.
/// </summary>
/// <param name="exact_matches">Element of class ExactMatches, which is a vector of ExactMatch (rules,type,string) for each exact match extracted from the snort rules' signatures</param>
/// <param name="keywords">An empty vector in which the basic_strings will be stored</param>
void convertExactMatches(const ExactMatches& exact_matches, std::vector<bstring>& bstrings) {
    for (auto it = exact_matches.exact_matches->begin(); it != exact_matches.exact_matches->end(); ++it) {
        std::string hexString = (*it)->getExactMatch();
        //std::set<int> rules = (*it)->getRulesNumbers();
        //total_unique_rules.insert(rules.begin(), rules.end());
        bstrings.push_back(hexStringToBstring(hexString));
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

    // Now you can access elements in the JSON object
    //std::string signature = jsonObj["signature"];
    //std::vector<std::string> exact_match_str = jsonObj["exact_match"];
    std::vector<std::string> exact_match_hex = jsonObj["exact_match_hex"];
    std::vector<int> non_unique_rules = jsonObj["rules"];
    std::set<int> rules(non_unique_rules.begin(), non_unique_rules.end());
    //for (const auto& rule : jsonObj["rules"]) {
    //    rules.insert(static_cast<int>(rule));
    //}

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

#endif // !_PARSER_H
