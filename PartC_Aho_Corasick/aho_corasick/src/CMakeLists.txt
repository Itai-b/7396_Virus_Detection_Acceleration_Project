# CMakeList.txt : CMake project for cuckoohash, include source and define
cmake_minimum_required(VERSION 3.12)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable (aho_corasick_exe "main.cpp")

target_include_directories(aho_corasick_exe PRIVATE ${CMAKE_LIBRARY_PATH}/include)

target_link_libraries(aho_corasick_exe PRIVATE aho_corasick)
target_link_libraries(aho_corasick_exe PRIVATE nlohmann_json::nlohmann_json)

if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
	target_compile_options(aho_corasick_exe PRIVATE /W4 /permissive- /std:c++latest)
else()
	target_compile_options(aho_corasick_exe PRIVATE -Wall -pedantic -std=c++17)
endif()
