#include "Auxiliary.h"

#define SIMULATION_POINTER_VALUE 0  // use for simulation (see comment on architecture differences)

typedef uint32_t sid_size_type_;    // the size of an SID (Signature ID)
typedef std::set<int>* sid_list_ptr_type_;
// For the sake of simplicity we use a "regular" pointer, of which the size is 8Bytes (on our x64-bits architecture)
// However, the purpose of this structer is to be allocated on a designated x32-bits hardware, meaning each pointer will be 4Bytes.
typedef uint32_t theoretical_ptr_type_;
typedef uint32_t substring_4bytes_type_;
typedef uint64_t substring_8bytes_type_;

/// <summary>
/// Template function for running a generic test of inserting substrings with length = sizeof(K) to libcuckoo hash table.
/// </summary>
/// <typeparam name="K">Type of the key {uint8_t, uint16_t, uint32_t, uint64_t}</typeparam>
/// <typeparam name="V">Type of the value {uint..., Empty, std::set<int>*}</typeparam>
/// <typeparam name="H">Type of the hash function {CustomHash - recommended, std::hash<K> - not recommended, unexpected results}</typeparam>
/// <typeparam name="L">Length of substring (L = sizeof(K))</typeparam>
/// <typeparam name="G">Gap between 2 substrings when parsing an exact match for substrings</typeparam>
template<typename K, typename V, typename H = CustomHash, std::size_t L = sizeof(K), std::size_t G = SUBSTRING_DEFAULT_GAP>
void runTests(Statistics& stats, SubstringLogger& log, const ExactMatches& exact_matches, const std::size_t num_of_tests = NUMBER_OF_TESTS, bool isSimulation = true) {
    std::size_t table_sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512 };
    
    std::vector<Substring<K>> substrings;
    std::size_t num_of_unique_rules = parseExactMatches<K, G>(exact_matches, substrings, log);
    std::size_t num_of_substrings_duplicates = getTotalNumOfDups(substrings);
    
    std::cout << "Starting Test: L = " << L << " , G = " << G << ", " << "increasing table size "       \
        << "[" << std::dec << substrings.size() << " Substring(s), " << num_of_substrings_duplicates   \
        << " Duplicates]" << std::endl << std::endl;
    
    // Useful when trying to quickly produce the substrings logs
    if (num_of_tests <= 0) {
        return;
    }

    libcuckoo::cuckoohash_map<K, V, H>* hashTable;
    for (std::size_t table_size : table_sizes) {
        std::size_t sum_additional_size_bytes = 0;
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
            std::vector<Substring<K>> substrings_in_table;

            for (auto& iter : substrings) {
                K key = iter.substring;
                V value;

                // In theory, we would want to have the value set as the pointer to the rules
                // HOWEVER, since we work on x64-bit architecture PCs in contrast to the x32-bit architecture designated processor,
                // we would simulate the space consumption for the theoretical pointer, and implement the search function in more simplistic way.
                if (isSimulation) {
                    value = SIMULATION_POINTER_VALUE;
                }
                // UNCOMMENT IF USING x32-bits hardware(!)
                // else  {
                //    value = iter.rules;
                // }

                // Check if hash capacity reached test threshold for hash table size
                if (hashTable->capacity() * sizeof(std::pair<K, V>) >= table_size && hashTable->load_factor() >= MAX_LOAD_FACTOR) {
                    break;
                }

                // Insert entry(key,value) to hash table
                hashTable->insert(key, value);
                substrings_in_table.push_back(iter);

                unique_rules_inserted.insert(iter.rules->begin(), iter.rules->end());
                if (hashTable->load_factor() > max_lf) {
                    max_lf = hashTable->load_factor();
                }
            }
            delete hashTable;
            
            // TIME STAMP END: delete hash table
            auto timestamp_b = std::chrono::high_resolution_clock::now();
            auto test_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_b - timestamp_a).count();


            substrings_inserted = substrings_in_table.size();
            sum_load_factors += max_lf;     // TODO: check if to add occupancy or final load factor instead of max
            //sum_occupancy += num_of_elements_inserted / num_of_table_slots;
            sum_substrings_inserted += substrings_inserted;
            sum_unique_rules_covered += unique_rules_inserted.size();
            sum_runtime += test_runtime;

            // calculate additional size of the data structure
            std::size_t additional_size_bytes = 0;
            for (Substring<K> substring : substrings_in_table) {
                // each entry in the hashtable has a pointer to a list of SIDs that were triggered from the entry's key.
                // in theory, this list's size is the count of SIDs times the size of each SID + the size of the 'next' pointer in the list.
                additional_size_bytes += substring.rules->size() * (sizeof(sid_size_type_) + sizeof(theoretical_ptr_type_));
            }
            sum_additional_size_bytes += additional_size_bytes;
            
            std::cout << "Test " << i + 1 << "/" << num_of_tests << ". Runtime: " << test_runtime << "[ms]. " << "Covered: "  \
                << double(unique_rules_inserted.size()) / double(num_of_unique_rules) * 100 << "% of rules." << std::endl;
        }
        // Collect and Print Statistics    
        std::size_t hash_table_size = table_size;
        std::size_t additional_size = int((sum_additional_size_bytes / 1024) / num_of_tests);
        double avg_load_factor = double(sum_load_factors) / num_of_tests;
        double avg_number_of_rules_inserted = double(sum_unique_rules_covered) / num_of_tests;
        double percentage_of_rules_inserted = (avg_number_of_rules_inserted / num_of_unique_rules) * 100;
        double avg_number_of_substrings_inserted = double(sum_substrings_inserted) / num_of_tests;
        double percentage_of_all_substrings_inserted = (avg_number_of_substrings_inserted / substrings.size()) * 100;
        double hash_power = 0;  // TODO: implement
        double average_run_time = double(sum_runtime) / num_of_tests;

        TestStatistics test_data = {
                hash_table_size,
                additional_size,
                avg_load_factor,
                avg_number_of_rules_inserted,
                percentage_of_rules_inserted,
                avg_number_of_substrings_inserted,
                percentage_of_all_substrings_inserted,
                hash_power,
                average_run_time
        };
        stats.addData(test_data);
        
        std::cout << std::endl << std::dec << substrings.size() << " Substring(s) have been produced." << std::endl             \
            << avg_number_of_substrings_inserted << " Substring(s) were inserted to the hash table on average." << std::endl    \
            << percentage_of_rules_inserted << "% Rules were covered on average." << std::endl                                  \
            << percentage_of_all_substrings_inserted << "% of all Substrings were inserted on average." << std::endl            \
            << "Average load factor was: " << avg_load_factor << std::endl                                                      \
            << "Additional size of SID list was: " << additional_size << "[KB]." << std::endl                                   \
            << "Data was calculated over " << num_of_tests << " run(s) of cuckoo hash insertions with L = " << sizeof(K)        \
            << " and G = " << G << "." << std::endl << "Average insertion time: " << average_run_time << "[ms]." << std::endl   \
            << std::endl;
    }
}


/// <summary>
/// Template function for running a generic test of inserting substrings with length = sizeof(K) to libcuckoo hash table.
/// </summary>
/// <typeparam name="K">Type of the key {uint8_t, uint16_t, uint32_t, uint64_t}</typeparam>
/// <typeparam name="V">Type of the value {uint..., Empty, std::set<int>*}</typeparam>
/// <typeparam name="H">Type of the hash function {CustomHash - recommended, std::hash<K> - not recommended, unexpected results}</typeparam>
/// <typeparam name="L">Length of substring (L = sizeof(K))</typeparam>
/// <typeparam name="G">Gap between 2 substrings when parsing an exact match for substrings</typeparam>
template<typename K, typename V, typename H = CustomHash, std::size_t L = sizeof(K), std::size_t G = SUBSTRING_DEFAULT_GAP>
void searchTest(std::string testString, SubstringLogger& log, const ExactMatches& exact_matches, bool isSimulation = true) {
    // TODO: sub testString with path to JSON testfile (to read many test strings for)
    Results results;
    std::vector<Substring<K>> substrings;
    // std::size_t num_of_unique_rules =
    parseExactMatches<K, G>(exact_matches, substrings, log);

    std::cout << "Starting Search Test: L = " << L << " , G = " << G << ", " << "[" << std::dec << substrings.size()    \
       << " Substring(s) were created]." << std::endl;

    libcuckoo::cuckoohash_map<K, V, H>* hashTable;
    std::size_t num_of_slots = substrings.size() * sizeof(std::pair<K, V>);
    //std::size_t sum_additional_size_bytes = 0;
    //double load_factor = 0;
    //double unique_rules_covered = 0;
    //double runtime = 0;
    
    std::shuffle(substrings.begin(), substrings.end(), std::default_random_engine(std::random_device()()));

    // TIME STAMP BEGIN: initiate hash table
    auto timestamp_a = std::chrono::high_resolution_clock::now();

    // Allocate a new cuckoo hash table for load factor consistancy
    hashTable = new libcuckoo::cuckoohash_map<K, V, H>(num_of_slots);
    hashTable->reserve(num_of_slots);
   
    // Inserting the substrings from the substrings vector to the hash table
    int substrings_inserted = 0;
    //double max_lf = 0.0;
    std::set<int> unique_rules_inserted;
    std::vector<Substring<K>> substrings_in_table;

    // Insert all substrings to the hash table
    for (auto& iter : substrings) {
        K key = iter.substring;
        V value;

        // In theory, we would want to have the value set as the pointer to the rules
        // HOWEVER, since we work on x64-bit architecture PCs in contrast to the x32-bit architecture designated processor,
        // we would simulate the space consumption for the theoretical pointer, and implement the search function in more simplistic way.
        if (isSimulation) {
            value = SIMULATION_POINTER_VALUE;
        }
        // UNCOMMENT IF USING x32-bits hardware(!)
        // else  {
        //    value = iter.rules;
        // }

        // NO NEED WHEN INSERTING ALL SUBSTRINGS (!) (Check if hash capacity reached test threshold for hash table size)
        //if (hashTable->capacity() * sizeof(std::pair<K, V>) >= table_size && hashTable->load_factor() >= MAX_LOAD_FACTOR) {
        //    break;
        //}

        // Insert entry(key,value) to hash table
        hashTable->insert(key, value);
        substrings_in_table.push_back(iter);

        unique_rules_inserted.insert(iter.rules->begin(), iter.rules->end());
        //if (hashTable->load_factor() > max_lf) {
        //    max_lf = hashTable->load_factor();
        //}
    }
    substrings_inserted = substrings_in_table.size();
    std::cout << "Hash Table created! " << substrings_inserted << " Substring(s) were inserted." << std::endl;


    // TODO: start a for loop for each test search        
    std::map<int, int> sid_hits_histogram;
    
    // Parse testString to extract substrings with relevant Lengths and Gaps respectively to the hash table:
    // It is assumed for now that the testString is in the form of "0xFFFFFFFF..."
    std::vector<Substring<K>> testSubstrings;
    std::set<int> empty_ruleset;  // used since the testSubstrings has *NO* rules related to it (unlike the substring entries in the hash table).
    Substring<K>::extractSubstrings(testString, testSubstrings, empty_ruleset, G, L, true);
    for (Substring<K> testSubstring : testSubstrings) {
        bool found = false;
        K key_to_search = testSubstring.substring;
        sid_list_ptr_type_ rules_ptr = nullptr;
        if (isSimulation) {
            found = hashTable->contains(key_to_search);
        }
        else {
            // now the value_found is the ptr for the list of rules
            found = true;
            try {
                hashTable->find(key_to_search);
            }
            catch (std::out_of_range) {
                found = false;
            }
            //found = (rules_ptr != nullptr);
        }
        
        // Deals with any hits to the given search pattern
        // Since we only simulated the rules Bloom Filter in Part D,
        //      we dont have in the real hash table the sid assosiated with every cuckoo hash table entry.
        // Iterates over the orignial substrings vector to get this information for further analysis.
        if (found) {
            // std::cout << "Found substring: " << testSubstring << std::endl;
            for (auto& substring : substrings) {
                if (substring == testSubstring) {
                    rules_ptr = substring.rules;
                    if (rules_ptr != nullptr) {
                        // std::cout << "With rules: ";
                        bool first = true;
                        for (auto rule : *rules_ptr) {
                            // Document the sid hit in the historgram
                            if (sid_hits_histogram.find(rule) == sid_hits_histogram.end()) {
                                sid_hits_histogram[rule] = 0;
                            }
                            sid_hits_histogram[rule]++;
                            // if (!first) {
                                // std::cout << ", ";
                            // }
                            // std::cout << rule;
                            //first = false;
                        }
                        // std::cout << std::endl;
                    }
                    else {
                        // we shouldn't get here
                        std::cout << "No rules found." << std::endl;
                    }
                }
            }
        }
    }
    // Document Test Search Results
    int i = 0; // the loop iteration (implement)
    std::cout << "Search Test Results: " << std::endl;
    std::cout << "Test String #" << i << " : " << sid_hits_histogram[original_sid] << " wanted hit(s)." << std::endl;

    SearchResults search_results = {
            search_key,
            original_sid,
            sid_hits_histogram
    };
    results.addData(search_results);
    // TODO: Close test loop

    // TIME STAMP END: delete hash table
    auto timestamp_b = std::chrono::high_resolution_clock::now();
    auto test_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_b - timestamp_a).count();


    // calculate additional size of the data structure
    std::size_t additional_size_bytes = 0;
    for (Substring<K> substring : substrings_in_table) {
        // each entry in the hashtable has a pointer to a list of SIDs that were triggered from the entry's key.
        // in theory, this list's size is the count of SIDs times the size of each SID + the size of the 'next' pointer in the list.
        additional_size_bytes += substring.rules->size() * (sizeof(sid_size_type_) + sizeof(theoretical_ptr_type_));
    }
    
    std::cout << "Finished search test. Time elapsed: " << test_runtime << "[ms]." << std::endl     \
        << "Table size: " << int(hashTable->capacity() * sizeof(std::pair<K, V>) / 1024) << "[KB]. "     \
        << "Additional size: " << int(additional_size_bytes / 1024) << "[KB]." << std::endl << std::endl;
    
    results.writeToFile("search_results.json"); // TODO: check file path
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
    // Define start time and path to: json file with all the rules, destination path to save the results, number of tests
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string file_path = "parta_data_by_exactmatch.json";
    std::string dest_path = "";
    std::string test_path = "";
    std::size_t num_of_tests = NUMBER_OF_TESTS;
    
    // Get arguments from VS/WSL
    getOpts(argc, argv, file_path, dest_path, &num_of_tests, test_path);
    
    // Parse PartA.json file to fill the ExactMatches vector with the extracted data from the .rules file
    ExactMatches exact_matches;
    parseFile(file_path, exact_matches); 

    // START TESTS:
    // Statistics stats_test1;
    SubstringLogger substrings_log0;
    std::string search_test_path = dest_path + "/SearchTMP_Length8_Gap1";
    std::string command = "mkdir -p " + search_test_path;
    //std::string command = "mkdir " + search_test_path;
    createDir(search_test_path);
    std::string testStr4 = "0x75736572"; // "user"
    std::string testStr8 = "0x75536552694E466F"; // "uSeRiNFo"
    std::string testStr8_2 = "0x674174454372615348455220762E312E322E33"; // "gAtECraSHER v1.2.3"
    searchTest<uint32_t, theoretical_ptr_type_, CustomHash, 4, 1>(testStr8_2, substrings_log0, exact_matches);
    //stats_test1.writeToFile(l8g1_path, "L8_G1_increasing_table_size.json");
    substrings_log0.writeToFile(search_test_path, "tmp.json");

    // DEBUG
    return 0;
    // DEBUG

    // Test 1: L = 8, G = 1
    Statistics stats_test1;
    SubstringLogger substrings_log1;
    std::string l8g1_path = dest_path + "/Length8_Gap1";
    command = "mkdir -p " + l8g1_path;
    system(command.c_str());
    runTests<uint64_t, theoretical_ptr_type_, CustomHash, 8, 1>(stats_test1, substrings_log1, exact_matches, num_of_tests);
    stats_test1.writeToFile(l8g1_path, "L8_G1_increasing_table_size.json");
    substrings_log1.writeToFile(l8g1_path, "L8_G1_substrings.json");

    // Test 2: L = 8, G = 2
    Statistics stats_test2;
    SubstringLogger substrings_log2;
    std::string l8g2_path = dest_path + "/Length8_Gap2";
    command = "mkdir -p " + l8g2_path;
    system(command.c_str());
    runTests<uint64_t, theoretical_ptr_type_, CustomHash, 8, 2>(stats_test2, substrings_log2, exact_matches, num_of_tests);
    stats_test2.writeToFile(l8g2_path, "L8_G2_increasing_table_size.json");
    substrings_log2.writeToFile(l8g2_path, "L8_G2_substrings.json");

    // Test 3: L = 4, G = 1
    Statistics stats_test3;
    SubstringLogger substrings_log3;
    std::string l4g1_path = dest_path + "/Length4_Gap1";
    command = "mkdir -p " + l4g1_path;
    system(command.c_str());
    runTests<uint32_t, theoretical_ptr_type_, CustomHash, 4, 1>(stats_test3, substrings_log3, exact_matches, num_of_tests);
    stats_test3.writeToFile(l4g1_path, "L4_G1_increasing_table_size.json");
    substrings_log3.writeToFile(l4g1_path, "L4_G1_substrings.json");

    // Test 4: L = 4, G = 2
    Statistics stats_test4;
    SubstringLogger substrings_log4;
    std::string l4g2_path = dest_path + "/Length4_Gap2";
    command = "mkdir -p " + l4g2_path;
    system(command.c_str());
    runTests<uint32_t, theoretical_ptr_type_, CustomHash, 4, 2>(stats_test4, substrings_log4, exact_matches, num_of_tests);
    stats_test4.writeToFile(l4g2_path, "L4_G2_increasing_table_size.json");
    substrings_log4.writeToFile(l4g2_path, "L4_G2_substrings.json");

    // Register finish time and calculate total execution time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() << " [ms]." << std::endl;
    
    return 0;
}