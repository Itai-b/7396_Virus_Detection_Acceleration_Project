#ifndef _PARSER_H
#define _PARSER_H

#include "ExactMatches.h"
#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

//template <typename T, std::size_t G>
//Substring<T> ExtractSubstring(std::vector<std::string> values) {
//    for (const auto& value : values) {
//        if 
//    }
//}

template <typename T, std::size_t G>
void parseLine(std::string line, ExactMatches* exact_matches) {
    int rule_id;
    std::string rule_type;
    vector<std::string> raw_matches;

    // clean '[' and ']' marks.
    line = line.substr(1, line.size() - 2);

    // parsing the line as a stringstream.
    std::stringstream ss(line);
    std::vector<std::string> values;

    while (std::getline(ss, line, ',')) {
        size_t start = line.find_first_not_of(" \t\n\r\f\v");
        size_t end = line.find_last_not_of(" \t\n\r\f\v");
        if (start != std::string::npos && end != std::string::npos) {
            values.push_back(line.substr(start, end - start + 1));
        }
    }
    
    rule_id = values[0];
    rule_type = values[1].substr(1, values[1].size() - 2);


    // Print the split values
    for (const auto& value : values) {
        std::cout << value << std::endl;
    }
}

#endif // !_PARSER_H
