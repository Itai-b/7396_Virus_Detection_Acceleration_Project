# CMakeList.txt : CMake project for cuckoohash, include source and define
cmake_minimum_required(VERSION 3.12)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable (aho_corasick "main.cpp" "aho_corasick.hpp" "Statistics.h" "Auxiliary.h" "Bstring.h")

target_include_directories(aho_corasick PRIVATE ${CMAKE_LIBRARY_PATH}/include)

target_link_libraries(aho_corasick PRIVATE nlohmann_json::nlohmann_json)

if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
	target_compile_options(aho_corasick PRIVATE /W4 /permissive- /std:c++latest)
else()
	target_compile_options(aho_corasick PRIVATE -Wall -pedantic -std=c++17)
endif()
