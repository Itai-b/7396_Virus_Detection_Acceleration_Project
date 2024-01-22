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
    nlohmann::json parsed_line = nlohmann::json::parse(line);
    int rule_id = parsed_line[0];
    std::string rule_type = parsed_line[1];
    std::vector<std::vector<std::string>> signatures = parsed_line[2];

    std::string match = "0x";

    int i = 0;
    for (const auto& signature : signatures ) {
        // for every exact match string
        int j = 0;
        for (const auto& hex : signature) {
            std::string value_to_concat = "";
            if ((hex.substr(0,2) == "0x")) {
                value_to_concat = hex.substr(2); 
            }
            match = match + value_to_concat;
            ++j;
        }
        //insert exact_match to ExactMatches.
        exact_matches.insert(ExactMatch(rule_id, rule_type, match));
        ++i;
        match = "0x";
    }
}

 /**
  * Parse a line in 'exact_matches_hex.json' to extract items of class ExactMatch.
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
