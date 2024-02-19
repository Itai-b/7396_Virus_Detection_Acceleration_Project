#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <nlohmann/json.hpp>

struct TestStatistics {
public:
    std::size_t hash_table_size;
    double load_factor;
    double avg_number_of_rules_inserted;
    double percentage_of_rules_inserted;
    double avg_number_of_substrings_inserted;
    double percentage_of_all_substrings_inserted;
    double hash_power;      // = 0 for tests without hash_power variations
    double average_run_time;
};

class Statistics {
public:
    Statistics() {}

    void addData(const TestStatistics& testStatistics) {
        allTestsData.push_back(testStatistics);
    }

    void writeToFile(const std::string& filename) {
        // Store the data from the vector to a JSON object
        nlohmann::json jsonData;
        for (const auto& test : allTestsData) {
            nlohmann::json dataItem;
            dataItem["hash_table_size"] = test.hash_table_size;
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
        std::ofstream outputFile(filename);
        if (outputFile.is_open()) {
            outputFile << std::setw(4) << jsonData; // Print with indentation of 4 spaces (= 1 tab)
            outputFile.close();
            std::cout << "Data has been written to " << filename << " successfully." << std::endl;
        }
        else {
            std::cerr << "Unable to open file " << filename << "." << std::endl;
        }
    }

private:
    std::vector<TestStatistics> allTestsData;
};

#endif // _STATISTICS_H
