#include "ExactMatches.h"
#include "Substring.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <libcuckoo/cuckoohash_map.hh>
#include <string>

//using namespace std;

using Substring64 = Substring<uint64_t>;
using Substring32 = Substring<uint32_t>;
using Substring16 = Substring<uint16_t>;
//using Substring8 = Substring<uint8_t>;

// 1) Parse the file to extract lines
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs

uint8_t convertHEX(std::string hex_str) {
    try {
        size_t pos;
        int intValue = std::stoi(hexString, &pos, 16);

        if (pos != hexString.length()) {
            std::cerr << "Invalid hexadecimal format.\n";
        }
        else {
            std::cout << "Converted integer value: " << intValue << "\n";
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << "\n";
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << "\n";
    }

    return 0;
}



void parseJSONIntoExactMatches(std::string filename, ExactMatches* exact_matches) {
    std::ifstream file(filename);
	std::string line;
	while (getline(file, line)) {
		parseLine(line, exact_matches);
	}
}

int main() {
    ExactMatches* exact_matches = new ExactMatches();
    std::string filename = "exact_matches.json";
    //std::string line = "[1, \"content\", [[\"2\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"\\u0006\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"d\", \"r\", \"i\", \"v\", \"e\", \"s\", \"$\", \"\\u0000\"]]]";
    std::string line = "[1, \"content\", [[\"0x32\", \"0x0\", \"0x0\", \"0x0\", \"0x6\", \"0x0\", \"0x0\", \"0x0\", \"0x64\", \"0x72\", \"0x69\", \"0x76\", \"0x65\", \"0x73\", \"0x24\", \"0x0\"]]]";
    parseLine(line, exact_matches);
    delete exact_matches;
    return 0;
}