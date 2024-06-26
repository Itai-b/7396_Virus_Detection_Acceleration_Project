#ifndef _CUCKOO_HASH_AUX_H      // Auxiliary.h
#define _CUCKOO_HASH_AUX_H

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
#include <typeinfo>
#include "ExactMatches.h"
#include "Substring.h"
#include "CustomHash.h"
#include "Parser.h"
#include "Statistics.h"
#include "Config.h"

// ENVIRONMENT DEFINITIONS
#if defined _MSC_VER         // RUNNING FROM VISUAL STUDIO (Windows Env)
#include <direct.h>

#elif defined __GNUC__      // RUNNING FROM WSL (GNU/Linux Env)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif      


// Creates a directory with system calls of windows (if running from Visual Studio) or linux (if running from WSL = GNU/Linux)
void createDir(std::string path) {
#if defined _MSC_VER
    _mkdir(path.data());
#elif defined __GNUC__
    //mkdir(path.data(), 0777);
    std::string command = "mkdir -p " + path;
    system(command.c_str());
#endif
}

// Gets the arguments for the main functions for either Visual Studio environment or WSL environment
void getOpts(int argc, char* argv[], std::string& file_path, std::string& dest_path, std::size_t* num_of_tests, std::string& test_path) {
    bool is_file_path_set = false;
#if defined _MSC_VER    // Visual Studio
    // Running from Visual Studio: get params from 'args' field in "launch.vs.json" (Debug -> Debug and Launch Settings for <project_name>)
    if (argc > 1) {
        file_path = argv[1];
        is_file_path_set = true;
    }
    if (argc > 2) {
        dest_path = argv[2];
    }
    if (argc > 3) {
        *num_of_tests = std::stoi(argv[3]);
    }
    if (argc > 4) {
        test_path = argv[4];
    }
#elif defined __GNUC__  // WSL (GNU/Linux)
    // Running from WSL: get params from "run_project_unix.sh" script
    int opt = 0;
    while ((opt = getopt(argc, argv, "f:d:n:t:")) != -1) {
        switch (opt) {
        case 'f':
            file_path = optarg;
            is_file_path_set = true;
            break;
        case 'd':
            dest_path = optarg;
            break;
        case 'n':
            *num_of_tests = static_cast<std::size_t>(std::stoi(std::string(optarg)));
            std::cout << "Number of tests: " << *num_of_tests << std::endl;
            break;
        case 't':
            test_path = optarg;
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-n num_of_tests] [-t test_path]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
#endif  
    if (!is_file_path_set) {
        std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-n num_of_tests]" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "File path: " << file_path << std::endl;
}
// END OF ENVIRONMENT DEFINITIONS

#endif                          // Auxiliary.h
