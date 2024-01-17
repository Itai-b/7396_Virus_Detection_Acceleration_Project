#include <libcuckoo/cuckoohash_map.hh>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//#include "ExactMatches.h"
//#include "Substring.h"

//using namespace std;
using json = nlohmann::json;

//using Substring64 = Substring<uint64_t>;
//using Substring32 = Substring<uint32_t>;
//using Substring16 = Substring<uint16_t>;
//using Substring8 = Substring<uint8_t>;

// 1) Parse the file to extract lines
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs


int main() {
    std::string filename = "exact_matches.json";
    //std::string line = "[1, \"content\", [[\"2\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"\\u0006\", \"\\u0000\", \"\\u0000\", \"\\u0000\", \"d\", \"r\", \"i\", \"v\", \"e\", \"s\", \"$\", \"\\u0000\"]]]";
    //std::string line = "[1, \"content\", [[\"0x32\", \"0x0\", \"0x0\", \"0x0\", \"0x6\", \"0x0\", \"0x0\", \"0x0\", \"0x64\", \"0x72\", \"0x69\", \"0x76\", \"0x65\", \"0x73\", \"0x24\", \"0x0\"]]]";
    std::string line = "[12, \"pcre\", [[\"0x67\", \"0x61\", \"0x74\", \"0x65\", \"0x63\", \"0x72\", \"0x61\", \"0x73\", \"0x68\", \"0x65\", \"0x72\"], [\"0x73\", \"0x65\", \"0x72\", \"0x76\", \"0x65\", \"0x72\"], [\"0x6f\", \"0x6e\", \"0x2d\", \"0x6c\", \"0x69\", \"0x6e\", \"0x65\", \"0x2e\", \"0x2e\", \"0x2e\"]]]";
    json parsed_json = json::parse(line);
    std::cout << "hello" << std::endl;
    std::vector<std::vector<std::string>> all_exact_matches_raw = parsed_json[2];
    
    int i = 0;
    
    for (const auto& pattern_group : all_exact_matches_raw ) {
        // for every exact match string
        std::cout << "Exact_match index: " << i << std::endl;
        int j = 0;
        for (const auto& match : pattern_group) {
            // go on every char
            std::cout << match << std::endl;
            ++j;
        }
        ++i;
    }
    return 0;
}