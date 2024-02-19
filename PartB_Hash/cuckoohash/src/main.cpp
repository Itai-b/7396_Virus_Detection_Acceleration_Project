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
#include "ExactMatches.h"
#include "Substring.h"
#include "CustomHash.h"
#include "Parser.h"
#include "Statistics.h"
#include "Config.h"

using json = nlohmann::json;

using Substring64 = Substring<uint64_t>;
using Substring32 = Substring<uint32_t>;
using Substring16 = Substring<uint16_t>;
using Substring8 = Substring<uint8_t>;
using Cuckoo64 = libcuckoo::cuckoohash_map<uint64_t, uint64_t, CustomHash>;     // TODO: change value type to Empty (size = 1) or find a way to insert keys only
using Cuckoo32 = libcuckoo::cuckoohash_map<uint32_t, uint32_t, CustomHash>;     // TODO: change value type to Empty or find a way to insert keys only
using Cuckoo16 = libcuckoo::cuckoohash_map<uint16_t, uint16_t, CustomHash>;     // TODO: change value type to Empty or find a way to insert keys only
using Cuckoo8 = libcuckoo::cuckoohash_map<uint8_t, uint8_t>;
using CuckooItem64 = std::pair<uint64_t, uint64_t>;
using CuckooItem32 = std::pair<uint32_t, uint32_t>;
using CuckooItem16 = std::pair<uint16_t, uint16_t>;

template <typename T, typename S>
int insertToCuckoo(const std::vector<Substring<T>>& substrings, libcuckoo::cuckoohash_map<T, S, CustomHash>& cuckoo_hash,
        std::size_t slots, std::size_t max_table_size, std::set<int>& unique_rules, double max_load_factor = MAX_LOAD_FACTOR) {
    int inserted = 0;
    double max_lf = 0.0;
    //unique_rules.clear();
    for (const auto& iter : substrings) {
        if (cuckoo_hash.capacity() * sizeof(std::pair<T,T>) >= max_table_size && cuckoo_hash.load_factor() >= max_load_factor) {
            /* DEBUG 
                 std::cout << "table size: " << cuckoo_hash.capacity() * sizeof(std::pair<T, T>) << " , load factor: " << cuckoo_hash.load_factor()      \
                << " , buckets: " << cuckoo_hash.bucket_count() << " , slots per bucket: " << cuckoo_hash.slot_per_bucket() << std::endl            \
                << "Max Load Factor was: " << std::fixed << std::setprecision(2) << max_lf << std::endl;
            */
            return inserted;
        }
        cuckoo_hash.insert(iter.substring, iter.substring);
        inserted++;
        unique_rules.insert(iter.rules->begin(), iter.rules->end());
        if (cuckoo_hash.load_factor() > max_lf) {
            max_lf = cuckoo_hash.load_factor();
        }
	}
    /* DEBUG
        std::cout << "Managed to fill all element(s)!" << std::endl << "table size: " << cuckoo_hash.capacity() * sizeof(std::pair<T, T>)           \
        << " , load factor: " << cuckoo_hash.load_factor() << " , buckets: " << cuckoo_hash.bucket_count() << " , slots per bucket: "               \
        << cuckoo_hash.slot_per_bucket() << std::endl << "Max Load Factor was: " << std::fixed << std::setprecision(2) << max_lf << std::endl;   
    */
    return inserted;
}

/// <summary>
/// Template function for running a generic test of inserting substrings with length = sizeof(K) to libcuckoo hash table.
/// </summary>
/// <typeparam name="K">Type of the key {uint8_t, uint16_t, uint32_t, uint64_t}</typeparam>
/// <typeparam name="V">Type of the value {uint..., Empty, std::set<int>*}</typeparam>
/// <typeparam name="H">Type of the hash function {CustomHash - recommended, std::hash<K> - not recommended, unexpected results}</typeparam>
/// <typeparam name="L">Length of substring (L = sizeof(K))</typeparam>
/// <typeparam name="G">Gap between 2 substrings when parsing an exact match for substrings</typeparam>
template<typename K, typename V = Empty, typename H = CustomHash, std::size_t L = sizeof(K), std::size_t G = SUBSTRING_DEFAULT_GAP>
void runTests(Statistics& stats, std::size_t table_sizes[], std::size_t max_hash_powers[] = nullptr) {
    std::vector<Substring<K>> substrings;
    std::size_t num_of_unique_rules = parseExactMatches<K>(exact_matches, substrings, G);
    
    std::cout << "Starting Test: L = " << L << " , G = " << G << ", ";
    if (max_hash_powers == nullptr) {
        std::cout << "increasing table size." << std::endl      \
            << "====================================================================================" << std::endl;
    }
    else {
        std::cout << "changing max hash power." << std::endl    \
            << "====================================================================================" << std::endl;
    }
    
    libcuckoo::cuckoohash_map<K, V, H>* hashTable;
    for (size_t table_size : table_sizes) {
        size_t num_of_slots = (table_size * 1024) / sizeof(std::pair<K, V>);
        double sum_load_factors = 0;
        //double sum_occupancy = 0;
        double sum_substrings_inserted = 0;
        double sum_unique_rules_covered = 0;
        double sum_runtime = 0;

        std::cout << "========================================== " << table_size << "[KB] ==========================================" << std::endl;
        for (int i = 0; i < NUMBER_OF_TESTS; ++i) {
            std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(std::random_device()()));
            //std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(SHUFFLE_SEED ^ static_cast<unsigned int>(i)));
            
            // TIME STAMP BEGIN: initiate hash table
            auto timestamp_a = std::chrono::high_resolution_clock::now();
            
            // Allocate a new cuckoo hash table for load factor consistancy
            hashTable = new libcuckoo::cuckoohash_map<K, V, H>(num_of_slots);
            hashTable->reserve(num_of_slots);
            // TODO: add hashpower changing
            //cuckoo_hash.maximum_hashpower(log2(MAX_TABLE_SIZE));
            

            // Inserting the substrings from the substrings vector to the hash table
            int substrings_inserted = 0;
            double max_lf = 0.0;
            std::set<int> unique_rules_inserted;

            for (const auto& iter : substrings) {
                if (hashTable->capacity() * sizeof(std::pair<K, V>) >= table_size && hashTable->load_factor() >= MAX_LOAD_FACTOR) {
                    break;
                }
                if (sizeof(V) == sizeof(K)) {
                    hashTable->insert(iter.substring, iter.substring);
                }
                else if (sizeof(V) == sizeof(Empty)) {
                    Empty e;
                    hashTable->insert(iter.substring, e);
                }
                else {
                    std::cout << "Error inserting to hash table: unexpected value size." << std::endl;
                    break;
                }
                substrings_inserted++;  // TODO: remove if the same as hashTable->size() in the end.
                unique_rules_inserted.insert(iter.rules->begin(), iter.rules->end());
                if (hashTable->load_factor() > max_lf) {
                    max_lf = hashTable->load_factor();
                }
            }
            double num_of_elements_inserted = double(hashTable->size());
            double num_of_table_slots = double(hashTable->capacity());
            // DEBUG
            assert(num_of_elements_inserted / num_of_table_slots == hashTable->load_factor());
            // if same, delete this part.
            // DEBUG
            delete hashTable;
            
            // TIME STAMP END: delete hash table
            auto timestamp_b = std::chrono::high_resolution_clock::now();
            auto test_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_b - timestamp_a).count();


            sum_load_factors += max_lf;     // TODO: check if to add occupancy or final load factor instead of max
            //sum_occupancy += num_of_elements_inserted / num_of_table_slots;
            sum_substrings_inserted += substrings_inserted;
            sum_unique_rules_covered += unique_rules_inserted.size();
            sum_runtime += test_runtime;
            // DEBUG
            assert(num_of_elements_inserted == substrings_inserted);
            
            std::cout << "Test " << i + 1 << "/" << NUMBER_OF_TESTS << ". Runtime: " << test_runtime << "[ms]. " << "Covered: "  \
                << double(unique_rules_inserted.size()) / double(num_of_unique_rules) * 100 << "% of rules." << std::endl;
            //DEBUG
        }
        // Collect and Print Statistics
        std::size_t hash_table_size = table_size;
        double avg_load_factor = double(sum_load_factors) / NUMBER_OF_TESTS;
        double avg_number_of_rules_inserted = double(sum_unique_rules_covered) / NUMBER_OF_TESTS;
        double percentage_of_rules_inserted = (avg_number_of_rules_inserted / num_of_unique_rules) * 100;
        double avg_number_of_substrings_inserted = double(sum_substrings_inserted) / NUMBER_OF_TESTS;
        double percentage_of_all_substrings_inserted = (avg_number_of_substrings_inserted / substrings.size()) * 100;
        double hash_power = 0;  // TODO: implement
        double average_run_time = double(sum_runtime) / NUMBER_OF_TESTS;
        TestStatistics test_data = {
            hash_table_size,
            avg_load_factor,
            avg_number_of_rules_inserted,
            percentage_of_rules_inserted,
            avg_number_of_substrings_inserted,
            percentage_of_all_substrings_inserted,
            hash_power,
            average_run_time
        };
        stats.addData(test_data);
        
        std::cout << std::endl << std::dec << substrings.size() << " Substring(s) have been produced." << std::endl                 \
            << avg_number_of_substrings_inserted << " Substring(s) were inserted to the hash table on average." << std::endl        \
            << percentage_of_rules_inserted << "% Rules were covered on average." << std::endl                                      \
            << percentage_of_all_substrings_inserted << "% of all Substrings were inserted on average." << std::endl                \
            << "Average load factor was : " << avg_load_factor << std::endl                                                         \
            << "Data was calculated over " << NUMBER_OF_TESTS << " run(s) of cuckoo hash insertions with L = " << sizeof(K)         \
            << " and G = " << G << "." << std::endl << "Average insertion time: " << average_run_time << "[ms]." << std::endl       \
            << std::endl;
    }
}

/// <summary>
/// Parse the .json file, which was generated by the python script in Part A, for ExactMatches.
/// Each ExactMatch includes the extracted sub-exact match from a given rule, the rule type (content / pcre) and relevant line number in the snort file.
/// 1) Parse the file to extract lines
//      line example:
//      //std::string line = "[12, \"pcre\", [[\"0x00\", \"0x00\", \"0x00\", \"0x65\", \"0x63\", \"0x72\", \"0x61\", \"0x73\", \"0x68\", \"0x65\", \"0x00\"], [\"0x73\", \"0x65\", \"0x72\", \"0x76\", \"0x65\", \"0x72\"], [\"0x6f\", \"0x6e\", \"0x2d\", \"0x6c\", \"0x69\", \"0x6e\", \"0x65\", \"0x2e\", \"0x2e\", \"0x2e\"]]]";
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs
/// </summary>
/// <param name="argc"></param>
/// <param name="argv">Run with path to exact_matches_hex.json from terminal/script.</param>
/// <returns></returns>
int main(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string file_path = "parta_data_by_exactmatch.json";
    if (argc >= 2) {    // running from console
        file_path = argv[1];
    }

    ExactMatches exact_matches;
    parseFile(file_path, exact_matches);
    
    std::vector<Substring64> substrings;
    std::size_t num_of_unique_rules = parseExactMatches<uint64_t>(exact_matches, substrings);
   
    std::size_t table_sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512}; // in KB
    /************ TEST 1: L=8, G=1, increasing table size ************/
    std::cout << "Starting Test 1: L=8, G=1, increasing table size" << std::endl << "===================================================" << std::endl;
    for (size_t table_size : table_sizes) {
        size_t num_of_slots = (table_size * 1024) / sizeof(CuckooItem64);
        Cuckoo64 cuckoo_hash(num_of_slots);
        cuckoo_hash.reserve(num_of_slots);
        //cuckoo_hash.maximum_hashpower(log2(MAX_TABLE_SIZE));
        //cuckoo_hash.max_num_worker_threads(0);
        assert(table_size == cuckoo_hash.capacity() * sizeof(CuckooItem64) / 1024);
        
        double sum_load_factors = 0;
        double sum_size = 0;
        double sum_inserted = 0;
        double sum_unique_rules_covered = 0;
        double sum_time = 0;
        
        std::cout << "========================================== " << table_size << "[KB] ==========================================" << std::endl;
        for (int i = 0; i < NUMBER_OF_TESTS; ++i) {
            std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(std::random_device()()));
            //std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(SHUFFLE_SEED ^ static_cast<unsigned int>(i)));
            std::set<int> unique_rules_inserted;
            auto timestamp_a = std::chrono::high_resolution_clock::now();
            int inserted = insertToCuckoo(substrings, cuckoo_hash, num_of_slots, table_size, unique_rules_inserted);
            auto timestamp_b = std::chrono::high_resolution_clock::now();
            auto test_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_b - timestamp_a).count();

            sum_load_factors += cuckoo_hash.load_factor();
            sum_size += cuckoo_hash.size() * sizeof(CuckooItem64);
            sum_inserted += inserted;
            sum_unique_rules_covered = unique_rules_inserted.size();
            sum_time += test_runtime;
            // DEBUG
            std::cout << "Test " << i + 1 << "/" << NUMBER_OF_TESTS << ". Runtime: " << test_runtime << "[ms]. " << "Covered: "  \
                << double(double(unique_rules_inserted.size()) / num_of_unique_rules) * 100 << "% of rules." << std::endl;
            //DEBUG
            

            /*std::cout << "Run " << (i + 1) << " out of " << NUMBER_OF_TESTS << " :" << std::endl    \
                << "Load factor of the hash is : " << cuckoo_hash.load_factor() << std::endl        \
                << "Number of Elements: " << cuckoo_hash.size() << std::endl                        \
                << "---------------------------------------------------" << std::endl;*/
            cuckoo_hash.clear();
        }
        // Print Statistics
        std::cout << std::endl << std::dec << substrings.size() << " Substring(s) have been produced." << std::endl                                     \
            << sum_inserted / NUMBER_OF_TESTS << " Substring(s) on average have been inserted to the Hash Table." << std::endl                          \
            << ((double(sum_unique_rules_covered) / NUMBER_OF_TESTS) / num_of_unique_rules) * 100 << "% rules were covered on average." << std::endl    \
            << ((sum_inserted / NUMBER_OF_TESTS) / substrings.size()) * 100 << "% of all Substrings were inserted on average." << std::endl             \
            << "Average load factor was : " << sum_load_factors / NUMBER_OF_TESTS << std::endl                                                          \
            << "Data was calculated over " << NUMBER_OF_TESTS << " run(s) of cuckoo hash insertions with L = "                                          \
            << sizeof(uint64_t) << " and G = " << SUBSTRING_DEFAULT_GAP << "." << std::endl                                                             \
            << "Average insertion time: " << sum_time / NUMBER_OF_TESTS << "[ms]." << std::endl << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}