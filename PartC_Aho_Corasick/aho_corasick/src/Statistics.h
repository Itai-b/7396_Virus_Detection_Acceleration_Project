#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cstdio>
#include <nlohmann/json.hpp>


struct TestStatistics {
public:
    std::size_t nodes_size;                         // an std::size_t representing the size (in [KB]) allocated to the aho corasick *NODES*
    std::size_t total_edges;                        // an std::size_t representing the total number of edges in the aho corasick TRIE
    std::size_t size_in_theory;                     // an std::size_t representing the theoretical size of the aho corasick TRIE in a minimalistic implementation
    std::size_t aho_corasick_size;                  // an std::size_t representing the size (in [KB]) allocated to the aho corasick TRIE (FULL)
    std::size_t aho_corasick_no_emits_size;         // an std::size_t representing the size (in [KB]) allocated to the aho corasick TRIE (Without Emits List)
    std::size_t exact_matches_inserted;             // an std::size_t representing the number of exact matches that were inserted to the aho corasick TRIE
    std::size_t threshold;                          // an std::size_t representing the min threshold of exact matches length that were inserted to the TRIE
    double run_time;                        // a double representing the average run time (in [ms]) of the test
};

/// <summary>
/// Class dedicated to store statistics from tests of inserting exact matches to the aho_corasick TRIE.
/// Stores the data from each test serie(s) to a json file.
/// </summary>
class Statistics {
public:
    Statistics() {}

    /// <summary>
    /// Usage: 
    ///     stats.addData({nodes_size, aho_corasick_size, aho_corasick_no_emits_size, exact_matches_inserted, threshold, average_run_time});
    /// </summary>
    /// <param name="testStatistics">A struct to contain the logged test statistics.</param>
    void addData(const TestStatistics& testStatistics) {
        allTestsData.push_back(testStatistics);
    }

    void writeToFile(const std::string& path, const std::string& filename) {
        // Store the data from the vector to a JSON object
        nlohmann::json jsonData;
        for (const auto& test : allTestsData) {
            nlohmann::json dataItem;
            dataItem["nodes_size"] = test.nodes_size;
            dataItem["total_edges"] = test.total_edges;
            dataItem["size_in_theory"] = test.size_in_theory;
            dataItem["aho_corasick_size"] = test.aho_corasick_size;
            dataItem["aho_corasick_no_emits_size"] = test.aho_corasick_no_emits_size;
            dataItem["exact_matches_inserted"] = test.exact_matches_inserted;
            dataItem["threshold"] = test.threshold;
            dataItem["run_time"] = test.run_time;
            jsonData.push_back(dataItem);
        }

        // Print the JSON object to a file
        std::string file_path = path + "/" + filename;
        std::ofstream outputFile(file_path);
        if (outputFile.is_open()) {
            outputFile << std::setw(4) << jsonData; // Print with indentation of 4 spaces (= 1 tab)
            outputFile.close();
            std::cout << "Written general statistics to " << filename << " successfully." << std::endl;
        }
        else {
            std::cerr << "Unable to open file " << file_path << "." << std::endl;
        }
    }

private:
    std::vector<TestStatistics> allTestsData;
};


/// Search Key, Original Rule, Rules Hits (#SIDs), # Hits on Original Rule, # Hits on Other Rules
struct SearchResults {
public:
    std::string search_key;                         // an std::string represents a search pattern that could be assosiated with a specific SID
    std::vector<int> original_sids;                 // a vector of integers that represent the Snort IDs of the wanted rules to search
    std::map<int, int> sids_hit;                    // a histogram of pairs (sid, number of hits)
    std::size_t size;                               // an std::size_t that represents the size of the data structure in Bytes
    std::size_t full_list_size;                     // number of bytes required to store all rules' SID(s) for all entries in the data structure
    std::size_t iblt_size_optimal;                  // number of bytes required for an iblt that ensures optimal success rate restoring all the rules for all entries in the data structure
    std::size_t iblt_size_100_rate;                 // number of bytes required for an iblt that ensures 100 success rate restoring all the rules for all entries in the data structure
    std::size_t iblt_size_99_rate;                  // number of bytes required for an iblt that ensures 99 success rate restoring all the rules for all entries in the data structure
    std::size_t iblt_size_95_rate;                  // number of bytes required for an iblt that ensures 95 success rate restoring all the rules for all entries in the data structure

};

/// <summary>
/// Class dedicated to store results from the search test in the aho corasick automaton.
/// Stores the data from each test to a json file.
/// </summary>
class Results {
public:
    Results() {}

    /// <summary>
    /// Usage: 
    ///     stats.addData({std::string search_key, int original_sid, std::vector<std::pair<int, int>> sids_hit})
    /// </summary>
    /// <param name="searchResults">A struct to contain the logged test results.</param>
    void addData(const SearchResults& searchResults) {
        results.push_back(searchResults);
    }

    void writeToFile(const std::string& path, const std::string& filename) {
        // Store the data from the vector to a JSON object
        nlohmann::json jsonData;
        for (const auto& data : results) {
            nlohmann::json dataItem;
            dataItem["search_key"] = data.search_key;
            dataItem["original_sids"] = data.original_sids;
            dataItem["sids_hit"] = data.sids_hit;
            dataItem["size"] = data.size;
            dataItem["additional_size_full_list"] = data.full_list_size;
            dataItem["additional_size_iblt_optimal"] = data.iblt_size_optimal;
            dataItem["additional_size_iblt_success_rate_100"] = data.iblt_size_100_rate;
            dataItem["additional_size_iblt_success_rate_99"] = data.iblt_size_99_rate;
            dataItem["additional_size_iblt_success_rate_95"] = data.iblt_size_95_rate;
            jsonData.push_back(dataItem);
        }

        // Print the JSON object to a file
        std::string file_path = path + "/" + filename;
        std::ofstream outputFile(file_path);
        if (outputFile.is_open()) {
            outputFile << std::setw(4) << jsonData; // Print with indentation of 4 spaces (= 1 tab)
            outputFile.close();
            std::cout << "Data has been written to " << filename << " successfully." << std::endl;
        }
        else {
            std::cerr << "Unable to open file " << file_path << "." << std::endl;
        }
    }

private:
    std::vector<SearchResults> results;
};



#endif // _STATISTICS_H
