#include <libcuckoo/cuckoohash_map.hh>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>
#include <functional> // DEBUG, for calling std::hash<>()(key)
#include "ExactMatches.h"
#include "Parser.h"
#include "Substring.h"

#define NUMBER_OF_TESTS 1

using json = nlohmann::json;

using Substring64 = Substring<uint64_t>;
using Substring32 = Substring<uint32_t>;
using Substring16 = Substring<uint16_t>;
using Substring8 = Substring<uint8_t>;
using Cuckoo64 = libcuckoo::cuckoohash_map<uint64_t, uint64_t>;
using Cuckoo32 = libcuckoo::cuckoohash_map<uint32_t, uint32_t>;
using Cuckoo16 = libcuckoo::cuckoohash_map<uint16_t, uint16_t>;
using Cuckoo8 = libcuckoo::cuckoohash_map<uint8_t, uint8_t>;

// 1) Parse the file to extract lines
//      line example:
//       //std::string line = "[12, \"pcre\", [[\"0x00\", \"0x00\", \"0x00\", \"0x65\", \"0x63\", \"0x72\", \"0x61\", \"0x73\", \"0x68\", \"0x65\", \"0x00\"], [\"0x73\", \"0x65\", \"0x72\", \"0x76\", \"0x65\", \"0x72\"], [\"0x6f\", \"0x6e\", \"0x2d\", \"0x6c\", \"0x69\", \"0x6e\", \"0x65\", \"0x2e\", \"0x2e\", \"0x2e\"]]]";
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs

template <typename T>
int insertToCuckoo(const std::vector<Substring<T>>& substrings, libcuckoo::cuckoohash_map<T, T>& cuckoo_hash) {
    int not_inserted = 0;
    double max_lf = 0.0;
    for (const auto& iter : substrings) {
        //if (cuckoo_hash.load_factor() <= libcuckoo::DEFAULT_MINIMUM_LOAD_FACTOR) {
        //    ++not_inserted;
        //}
        //else {
            //std::cout << "Hashed key value is: " << cuckoo_hash.hash_function()(iter.substring) << std::endl;
            cuckoo_hash.insert(iter.substring, iter.substring);
            //std::cout << "Load Factor is: " << cuckoo_hash.load_factor() << "." << std::endl; // TO CHECK WHAT IS THE HASH(KEY), ARE ALL SORTED TO SAME BUCKET(?)
            if (cuckoo_hash.load_factor() > max_lf) {
                max_lf = cuckoo_hash.load_factor();
            }
        //}
	}
    std::cout << "Max Load Factor was: " << std::fixed << std::setprecision(2) << max_lf << std::endl;
    return not_inserted;
}
//
//template <typename k, typename v>
//void printcuckoohash(const libcuckoo::cuckoohash_map<k, v>& cuckoo_hash) {
//    size_t bucket_index = 0;
//    for (const auto& bucket : cuckoo_hash) {
//        std::cout << "bucket " << bucket_index++ << ": ";
//        for (const auto& entry : bucket) {
//            std::cout << "(key: " << entry.first << ", value: " << entry.second << ") ";
//        }
//        std::cout << std::endl;
//    }
//}

template <typename T>
struct SubstringHash {
    std::size_t operator()(const T key) const {
        constexpr size_t num_of_bits = sizeof(T) * 8;
        constexpr size_t half_bits = num_of_bits / 2;
        T lower_half = key & ((T(1) << half_bits) - 1);     // keep lower half of key :(key) bitwise AND (0000...-1111...1)
        T flipped_higher_half = (~key) >> half_bits;          // keep NOT higher half of key
        std::size_t hashed_substring = static_cast<std::size_t>(lower_half ^ flipped_higher_half);
        
        return hashed_substring;    // bitwise XOR the uppd
    }
};

template <typename T>
struct SubstringXORHash {
    std::size_t operator()(const T substring) const {
        // TODO: Implement a simpler XOR.
        return 0;
    }
};

/// <summary>
/// Parse the .json file, which was generated by the python script in Part A, for ExactMatches.
/// Each ExactMatch includes the extracted sub-exact match from a given rule, the rule type (content / pcre) and relevant line number in the snort file.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string file_path = "exact_matches_hex.json";
    if (argc >= 2) { // running from console
        file_path = argv[1];
    }

    ExactMatches exact_matches;
    parseFile(file_path, exact_matches);
    
    // Parse the ExactMatches and extract Substrings of L bytes with parsing of G bytes jump gap per substring.
    // Create a vector for shuffling the values (we used set for removing doubles, but it cannot be shuffled directly).
    std::set<Substring64> substrings;
    for (auto it = exact_matches.exact_matches->begin(); it != exact_matches.exact_matches->end(); ++it) {
        std::string hexString = (*it)->getExactMatch().substr(2);   // clean "0x"
        Substring64::extractSubstrings(hexString, substrings);      // G = default 1, L = default sizeof(T), T here is uint64_t.
    }
    std::vector<Substring64> shuffled_substrings(substrings.begin(), substrings.end());
   
    // Make NUMBER_OF_TESTS random shuffles of the vector, for each, insert to a new cuckoohash and calculate the capacity
    const std::size_t TABLE_SIZE = 32 * 1024;
    const std::size_t SLOTS = TABLE_SIZE / sizeof(std::pair<uint64_t, uint64_t>);
    Cuckoo64 cuckoo_hash(SLOTS);
    double sum_load_factors = 0;
    double sum_not_inserted = 0;
    
    for (int i = 0; i < NUMBER_OF_TESTS; ++i) {
        std::shuffle(shuffled_substrings.begin(), shuffled_substrings.end(), std::default_random_engine(std::random_device()()));
        std::cout << "The initial size of the hash is: " << cuckoo_hash.size() << ", but was initiallized to: " << SLOTS << std::endl;
        sum_not_inserted += insertToCuckoo(shuffled_substrings, cuckoo_hash);
        sum_load_factors += cuckoo_hash.load_factor();
        std::cout << "The load factor of the hash is: " << cuckoo_hash.load_factor() << std::endl;
        std::cout << "The final size of the hash is: " << cuckoo_hash.size() << std::endl;
        cuckoo_hash.clear();
    }

    // Print Statistics
    // TODO: check why load factor stays the same. (also if changing NUMBER_OF_RESERVED_ELEMENTS to a random LOW number like 256).
    std::cout << "Average load factor is: " << sum_load_factors / NUMBER_OF_TESTS << std::endl \
        << "Average elements not inserted is: " << sum_not_inserted / NUMBER_OF_TESTS << std::endl \
        << " Data was calculated of " << NUMBER_OF_TESTS << " run(s) of cuckoo hash insertions with L = " \
        << sizeof(Substring64) << " and G = " << _SUBSTRING_DEFAULT_GAP << "." << std::endl;
    
    std::cout << std::dec << substrings.size() << " Substring(s) have been produced." << std::endl;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Execution time recorded: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}