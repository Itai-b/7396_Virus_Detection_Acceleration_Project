#include <libcuckoo/cuckoohash_map.hh>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>
#include "ExactMatches.h"
#include "Parser.h"
//#include "Substring.h"

//using namespace std;
using json = nlohmann::json;

using Substring64 = Substring<uint64_t>;
using Substring32 = Substring<uint32_t>;
using Substring16 = Substring<uint16_t>;
using Substring8 = Substring<uint8_t>;

// 1) Parse the file to extract lines
//      line example:
//       //std::string line = "[12, \"pcre\", [[\"0x00\", \"0x00\", \"0x00\", \"0x65\", \"0x63\", \"0x72\", \"0x61\", \"0x73\", \"0x68\", \"0x65\", \"0x00\"], [\"0x73\", \"0x65\", \"0x72\", \"0x76\", \"0x65\", \"0x72\"], [\"0x6f\", \"0x6e\", \"0x2d\", \"0x6c\", \"0x69\", \"0x6e\", \"0x65\", \"0x2e\", \"0x2e\", \"0x2e\"]]]";
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desi`red G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs


int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    // TODO: when writing the unix_script, main will take argv argc of the full file path, GIVEN BY THE UNIX SCRIPT (bash: ./main_exe "$WORKDIR/$SUBDIR/...")
    std::string filename = "exact_matches_hex.json";
    ExactMatches exact_matches;
    parseFile(filename, exact_matches);


    std::set<Substring64> substrings;
    //TODO: write an iterator for class ExactMatches
    //  OR: GET RID OF EXACTMATCHE*S* class, change name to ExactMatch.h because EXACTMATCHE*S* is just a vector (redundant implementation)
    // if so, change also parser accordingly (or should we keep it that way for eNcApSuLatIon
    // i mean, vector already allocates and frees what it uses...
    for (auto it = exact_matches.exact_matches->begin(); it != exact_matches.exact_matches->end(); ++it)
    {
        std::string hexString = (*it)->getExactMatch().substr(2); // clean "0x"
        Substring64::extractSubstrings(hexString, substrings); // G = default 1, L = default sizeof(T), T here is uint64_t.
    }
    
    // Create a vector and copy elements from the set
    std::vector<Substring64> shuffledSubstrings(substrings.begin(), substrings.end());
    

    // Can also do it on the set itself, but then technically we aren't starting from the same starting point always(?)
    //std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(std::random_device()()));
    
    // Shuffle the vector
    std::shuffle(shuffledSubstrings.begin(), shuffledSubstrings.end(), std::default_random_engine(std::random_device()()));

    // Display the shuffled elements (MAKES PROGRAM VERY SLOW, CHECK WAYS TO SPEED UP PRINT IN OPERATOR<<.
    //for (const auto& substring : shuffledSubstrings) {
    //    std::cout << substring << std::endl;
    //}

    std::cout << std::dec << shuffledSubstrings.size() << " Substring(s) have been produced." << std::endl;


    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Execution time recorded: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}