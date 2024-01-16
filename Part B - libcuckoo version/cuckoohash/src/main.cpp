#include "ExactMatches.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <libcuckoo/cuckoohash_map.hh>
#include <string>

using namespace std;

void parseLine(string line, ExactMatches* exact_matches) {
    line = line.substr(1, line.size() - 2);

    std::stringstream ss(line);

    std::vector<std::string> values;

    while (std::getline(ss, line, ',')) {
        size_t start = line.find_first_not_of(" \t\n\r\f\v");
        size_t end = line.find_last_not_of(" \t\n\r\f\v");
        if (start != std::string::npos && end != std::string::npos) {
            values.push_back(line.substr(start, end - start + 1));
        }
    }

    // Print the split values
    for (const auto& value : values) {
        std::cout << value << std::endl;
    }
}

void parseJSONIntoExactMatches(string filename, ExactMatches* exact_matches) {
    ifstream file(filename);
	string line;
	while (getline(file, line)) {
		parseLine(line, exact_matches);
	}
}

int main() {
    ExactMatches* exact_matches = new ExactMatches();
    string filename = "exact_matches.json";
    string line = "[1, \"content\", [[\"2\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"\\u0006\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"d\", \"r\", \"i\", \"v\", \"e\", \"s\", \"$\", \"\\u0000\"]]]";
    parseLine(line, exact_matches);
    delete exact_matches;
    return 0;
}