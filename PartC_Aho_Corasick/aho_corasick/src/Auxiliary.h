#ifndef _AHO_CORASICK_AUX_H      // Auxiliary.h
#define _AHO_CORASICK_AUX_H

#include "aho_corasick.hpp"
#include "Parser.h"
#include "Statistics.h"
#include "ExactMatches.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <chrono>

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
void getOpts(int argc, char* argv[], std::string& file_path, std::string& dest_path, std::string& test_path) {
    bool is_file_path_set = false;
    bool is_test_path_set = false;
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
        test_path = argv[3];
        is_test_path_set = true;
    }
#elif defined __GNUC__  // WSL (GNU/Linux)
    // Running from WSL: get params from "run_project_unix.sh" script
    int opt = 0;
    while ((opt = getopt(argc, argv, "f:d:t:")) != -1) {
        switch (opt) {
        case 'f':
            file_path = optarg;
            is_file_path_set = true;
            break;
        case 'd':
            dest_path = optarg;
            break;
        case 't':
            test_path = optarg;
            is_test_path_set = true;
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-t test_file_path]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
#endif  
    if (!is_file_path_set || !is_test_path_set) {
        std::cerr << "Usage: " << argv[0] << " [-f file_path] [-d dest_path] [-t test_file_path]" << std::endl;
        exit(EXIT_FAILURE);
}
    std::cout << "File path: " << file_path << std::endl;
}
// END OF ENVIRONMENT DEFINITIONS

#endif                          // Auxiliary.h
