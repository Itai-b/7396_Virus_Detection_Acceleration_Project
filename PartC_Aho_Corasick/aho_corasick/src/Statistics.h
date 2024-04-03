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
            std::cout << "Data has been written to " << filename << " successfully." << std::endl;
        }
        else {
            std::cerr << "Unable to open file " << file_path << "." << std::endl;
        }
    }

private:
    std::vector<TestStatistics> allTestsData;
};

#endif // _STATISTICS_H
