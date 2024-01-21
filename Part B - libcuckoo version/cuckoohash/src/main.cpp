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
#include "Substring.h"

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
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs

template <typename T>
void insertToCuckoo(const std::vector<Substring<T>>& substrings, libcuckoo::cuckoohash_map<T, std::size_t>& cuckoo_hash) {
	for (const auto& iter : substrings) {
		cuckoo_hash.insert(iter.substring, 1);
	}
}

int main(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::string file_path = "exact_matches_hex.json";
    if (argc >= 2) {
		file_path = argv[1];
	}
    
    ExactMatches exact_matches;
    parseFile(file_path, exact_matches);

    std::set<Substring64> substrings;
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

    // Create cuckoo hash map
    libcuckoo::cuckoohash_map<uint64_t, std::size_t> cuckoo_hash;
    cuckoo_hash.reserve(shuffledSubstrings.size());
    insertToCuckoo(shuffledSubstrings, cuckoo_hash);

    std::cout << "The capacity of the hash is: " << cuckoo_hash.load_factor() << std::endl;

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Execution time recorded: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}