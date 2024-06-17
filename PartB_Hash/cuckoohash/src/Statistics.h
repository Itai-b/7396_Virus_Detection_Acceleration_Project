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
    std::size_t hash_table_size;                    // an std::size_t represents the size (in [KB]) allocated to the hash table tested
    std::size_t additional_size;                    // an std::size_t represents the additional size (in [KB]) allocated for the lists of SID for each entry of the hash table.
    double load_factor;                             // a double represents the load factor of the hash table in the end of the test
    double avg_number_of_rules_inserted;            // a double represents the average number of rules inserted to the hash table
    double percentage_of_rules_inserted;            // a double represents the percentage of rules inserted to the hash table
    double avg_number_of_substrings_inserted;       // a double represents the average number of substrings inserted to the hash table
    double percentage_of_all_substrings_inserted;   // a double represents the percentage of substrings inserted to the hash table
    double hash_power;                              // a double represents the pre-determined of the hash table tested (0 = not restricted)
    double average_run_time;                        // a double represents the average run time (in [ms]) of the test
};

/// <summary>
/// Class dedicated to store statistics from tests of inserting substrings to cuckoo hash map.
/// Stores the data from each test serie(s) to a json file.
/// </summary>
class Statistics {
public:
    Statistics() {}

    /// <summary>
    /// Usage: 
    ///     stats.addData({hash_table_size, load_factor, avg_number_of_rules_inserted, percentage_of_rules_inserted,            
    ///         avg_number_of_substrings_inserted, percentage_of_all_substrings_inserted, hash_power, average_run_time});
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
            dataItem["hash_table_size"] = test.hash_table_size;
            dataItem["additional_size"] = test.additional_size;
            dataItem["load_factor"] = test.load_factor;
            dataItem["number_of_rules_inserted"] = test.avg_number_of_rules_inserted;
            dataItem["percentage_of_rules_inserted"] = test.percentage_of_rules_inserted;
            dataItem["number_of_substrings_inserted"] = test.avg_number_of_substrings_inserted;
            dataItem["percentage_of_all_substrings_inserted"] = test.percentage_of_all_substrings_inserted;
            dataItem["hash_power"] = test.hash_power;
            dataItem["average_run_time"] = test.average_run_time;
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


/// Search Key, Original Rule, Rules Hits (#SIDs), # Hits on Original Rule, # Hits on Other Rules
struct SearchResults {
public:
    std::string search_key;                         // an std::string represents a search pattern that could be assosiated with a specific SID
    int original_sid;                               // an integer represents the Snort ID of the wanted rule to search
    std::vector<std::pair<int,int>> sids_hit;       // a histogram of pairs (sid, number of hits)
};

/// <summary>
/// Class dedicated to store results from the search test in the cuckoo hash map.
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
            dataItem["original_sid"] = data.original_sid;
            dataItem["sids_hit"] = data.sids_hit;
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



struct SubstringData {
public:
    std::size_t uint_representation;        // an std::size_t of the substring unsigned int value
    std::string hex_representation;         // an std::string of the substring in hex representation
    std::string full_representation;        // an std::string of the substring in full representation 
    std::size_t num_of_duplicates;          // an std::size_t of the number of same substrings duplicates removed
    std::set<int> rules;                    // an std::set<int> of the rules this substring represents
};

/// <summary>
/// Class dedicated to log the substrings that were extracted to a .json file.
/// Mainly used to debugging and logging of processed data.
/// </summary>
class SubstringLogger {
public:
    SubstringLogger() {}


    /// <summary>
    /// Usage:
    ///     log.logSubstringData({uint_representation, hex_representation, 
    ///         full representation, num_of_duplicates, rules});                   
    /// </summary>
    /// <param name="item">A struct to contain the logged substrings.</param>
    void logSubstringData(const SubstringData& item) {
        data.push_back(item);
    }

    void writeToFile(const std::string& path, const std::string& filename) {
        // Store the data from the vector to a JSON object
        nlohmann::json jsonData;
        for (const auto& item : data) {
            nlohmann::json dataItem;
            dataItem["uint_key"] = item.uint_representation;
            dataItem["hex_representation"] = item.hex_representation;
            dataItem["full_representation"] = item.full_representation;
            dataItem["num_of_duplicates"] = item.num_of_duplicates;
            dataItem["rules"] = item.rules;
            jsonData.push_back(dataItem);
        }

        // Print the JSON object to a file
        std::string file_path = path + "/" + filename;
        std::ofstream outputFile(file_path);
        if (outputFile.is_open()) {
            outputFile << std::setw(4) << jsonData; // Print with indentation of 4 spaces (= 1 tab)
            outputFile.close();
            std::cout << "Substring(s) have been logged in " << file_path << " successfully." << std::endl;
        }
        else {
            std::cerr << "Unable to open file " << file_path << "." << std::endl;
        }
    }

private:
    std::vector<SubstringData> data;
};


#endif // _STATISTICS_H
