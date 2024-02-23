#include <libcuckoo/cuckoohash_map.hh>
#include <nlohmann/json.hpp>
#include <unistd.h>
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


/// <summary>
/// Template function for running a generic test of inserting substrings with length = sizeof(K) to libcuckoo hash table.
/// </summary>
/// <typeparam name="K">Type of the key {uint8_t, uint16_t, uint32_t, uint64_t}</typeparam>
/// <typeparam name="V">Type of the value {uint..., Empty, std::set<int>*}</typeparam>
/// <typeparam name="H">Type of the hash function {CustomHash - recommended, std::hash<K> - not recommended, unexpected results}</typeparam>
/// <typeparam name="L">Length of substring (L = sizeof(K))</typeparam>
/// <typeparam name="G">Gap between 2 substrings when parsing an exact match for substrings</typeparam>
template<typename K, typename V, typename H = CustomHash, std::size_t L = sizeof(K), std::size_t G = SUBSTRING_DEFAULT_GAP>
void runTests(Statistics& stats, SubstringLogger& log, const ExactMatches& exact_matches, const std::size_t num_of_tests = NUMBER_OF_TESTS) {
    std::size_t table_sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512 };
    
    std::vector<Substring<K>> substrings;
    std::size_t num_of_unique_rules = parseExactMatches<K, G>(exact_matches, substrings, log);
    std::size_t num_of_substrings_duplicates = getTotalNumOfDups(substrings);
    
    std::cout << "Starting Test: L = " << L << " , G = " << G << ", " << "increasing table size "       \
        << "[" << std::dec << substrings.size() << " Substring(s), " << num_of_substrings_duplicates   \
        << " Duplicates]" << std::endl << std::endl;
    /* if (max_hash_powers == nullptr)
        std::cout << "increasing table size." << std::endl;
    else {
        std::cout << "changing max hash power." << std::endl;
    }*/
    
    // Useful when trying to quickly produce the substrings logs
    if (num_of_tests <= 0) {
        return;
    }

    libcuckoo::cuckoohash_map<K, V, H>* hashTable;
    for (std::size_t table_size : table_sizes) {
        std::size_t num_of_slots = (table_size * 1024) / sizeof(std::pair<K, V>);
        double sum_load_factors = 0;
        double sum_substrings_inserted = 0;
        double sum_unique_rules_covered = 0;
        double sum_runtime = 0;

        std::cout << "========================================== " << table_size << "[KB] ==========================================" << std::endl;
        for (std::size_t i = 0; i < num_of_tests; ++i) {
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
                // TODO: fix this empty thing
                //else if (sizeof(V) == sizeof(Empty)) {
                //    Empty e;
                //    hashTable->insert(iter.substring, e);
                //}
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
            
            std::cout << "Test " << i + 1 << "/" << num_of_tests << ". Runtime: " << test_runtime << "[ms]. " << "Covered: "  \
                << double(unique_rules_inserted.size()) / double(num_of_unique_rules) * 100 << "% of rules." << std::endl;
            //DEBUG
        }
        // Collect and Print Statistics    
        std::size_t hash_table_size = table_size;
        double avg_load_factor = double(sum_load_factors) / num_of_tests;
        double avg_number_of_rules_inserted = double(sum_unique_rules_covered) / num_of_tests;
        double percentage_of_rules_inserted = (avg_number_of_rules_inserted / num_of_unique_rules) * 100;
        double avg_number_of_substrings_inserted = double(sum_substrings_inserted) / num_of_tests;
        double percentage_of_all_substrings_inserted = (avg_number_of_substrings_inserted / substrings.size()) * 100;
        double hash_power = 0;  // TODO: implement
        double average_run_time = double(sum_runtime) / num_of_tests;

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
            << "Data was calculated over " << num_of_tests << " run(s) of cuckoo hash insertions with L = " << sizeof(K)             \
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
    std::string dest_path = "";
    std::size_t num_of_tests = NUMBER_OF_TESTS;
    
    int opt;
    bool is_file_path_set = false;
    while ((opt = getopt(argc, argv, "f:d:n:")) != -1) {
        switch (opt) {
        case 'f':
            file_path = optarg;
            is_file_path_set = true;
            break;
        case 'd':
            dest_path = optarg;
            break;
        case 'n':
            num_of_tests = static_cast<std::size_t>(std::stoi(std::string(optarg)));
            std::cout << "Number of tests: " << num_of_tests << std::endl;
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-n num_of_tests]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if (!is_file_path_set) {
        std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-n num_of_tests]" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "File path: " << file_path << std::endl;
    ExactMatches exact_matches;
    parseFile(file_path, exact_matches); 

    Statistics stats_test1;
    SubstringLogger substrings_log1;
    std::string l8g1_path = dest_path + "/Length8_Gap1";
    std::string command = "mkdir -p " + l8g1_path;
    system(command.c_str());
    runTests<uint64_t, uint64_t, CustomHash, 8, 1>(stats_test1, substrings_log1, exact_matches, num_of_tests);
    stats_test1.writeToFile(l8g1_path, "L8_G1_increasing_table_size.json");
    substrings_log1.writeToFile(l8g1_path, "L8_G1_substrings.json");

    Statistics stats_test2;
    SubstringLogger substrings_log2;
    std::string l8g2_path = dest_path + "/Length8_Gap2";
    command = "mkdir -p " + l8g2_path;
    system(command.c_str());
    runTests<uint64_t, uint64_t, CustomHash, 8, 2>(stats_test2, substrings_log2, exact_matches, num_of_tests);
    stats_test2.writeToFile(l8g2_path, "L8_G2_increasing_table_size.json");
    substrings_log2.writeToFile(l8g2_path, "L8_G2_substrings.json");

    Statistics stats_test3;
    SubstringLogger substrings_log3;
    std::string l4g1_path = dest_path + "/Length4_Gap1";
    command = "mkdir -p " + l4g1_path;
    system(command.c_str());
    runTests<uint32_t, uint32_t, CustomHash, 4, 1>(stats_test3, substrings_log3, exact_matches, num_of_tests);
    stats_test3.writeToFile(l4g1_path, "L4_G1_increasing_table_size.json");
    substrings_log3.writeToFile(l4g1_path, "L4_G1_substrings.json");

    Statistics stats_test4;
    SubstringLogger substrings_log4;
    std::string l4g2_path = dest_path + "/Length4_Gap2";
    command = "mkdir -p " + l4g2_path;
    system(command.c_str());
    runTests<uint32_t, uint32_t, CustomHash, 4, 2>(stats_test4, substrings_log4, exact_matches, num_of_tests);
    stats_test4.writeToFile(l4g2_path, "L4_G2_increasing_table_size.json");
    substrings_log4.writeToFile(l4g2_path, "L4_G2_substrings.json");

    /*Statistics stats_test2;
    runTests<uint64_t, Empty, CustomHash, 8, 1>(stats_test2, exact_matches);
    stats_test2.writeToFile("L8_G1_EmptyClass_increasing_table_size.json");

    Statistics stats_test3;
    runTests<uint64_t, uint64_t, CustomHash, 8, 2>(stats_test3, exact_matches);
    stats_test3.writeToFile("L8_G2_increasing_table_size.json");

    Statistics stats_test4;
    runTests<uint64_t, Empty, CustomHash, 8, 2>(stats_test4, exact_matches);
    stats_test4.writeToFile("L8_G2_EmptyClass_increasing_table_size.json");
    
    Statistics stats_test5;
    runTests<uint32_t, uint32_t, CustomHash, 4, 1>(stats_test5, exact_matches);
    stats_test5.writeToFile("L4_G1_increasing_table_size.json");

    Statistics stats_test6;
    runTests<uint32_t, Empty, CustomHash, 4, 1>(stats_test6, exact_matches);
    stats_test6.writeToFile("L4_G1_EmptyClass_increasing_table_size.json");

    Statistics stats_test7;
    runTests<uint32_t, uint32_t, CustomHash, 4, 2>(stats_test7, exact_matches);
    stats_test7.writeToFile("L4_G2_increasing_table_size.json");

    Statistics stats_test8;
    runTests<uint32_t, Empty, CustomHash, 4, 2>(stats_test8, exact_matches);
    stats_test8.writeToFile("L4_G1_EmptyClass_increasing_table_size.json");*/

    // TODO: Add HashPower test

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}