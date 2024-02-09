#ifndef _PARSER_H
#define _PARSER_H

#include "ExactMatches.h"
#include <nlohmann/json.hpp>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>

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
    std::string signature = jsonObj["signature"];
    std::string signature_type = jsonObj["signature_type"];
    std::vector<int> rules = jsonObj["rules"];
    std::vector<std::vector<std::string>> exact_matches_str = jsonObj["exact_matches"];
    std::vector<std::vector<std::string>> exact_matches_hex = jsonObj["exact_matches_hex"];

    std::string match = "0x";

    int i = 0;
    for (const auto& exact_match_hex : exact_matches_hex) {
        // for every exact match string
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
        exact_matches.insert(ExactMatch(rules, signature_type, match));
        ++i;
        match = "0x";
    }
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
