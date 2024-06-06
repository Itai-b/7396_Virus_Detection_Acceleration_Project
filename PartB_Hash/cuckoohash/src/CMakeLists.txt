# CMakeList.txt : CMake project for cuckoohash, include source and define
cmake_minimum_required(VERSION 3.12)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable (cuckoohash "main.cpp" "CustomHash.h" "Statistics.h" "Config.h" "Auxiliary.h")

#find_package(libcuckoo REQUIRED)
#find_package(nlohmann_json REQUIRED)
target_include_directories(cuckoohash PRIVATE ${CMAKE_LIBRARY_PATH}/include)

target_link_libraries(cuckoohash PRIVATE libcuckoo -lpthread)
target_link_libraries(cuckoohash PRIVATE nlohmann_json::nlohmann_json)

if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
	target_compile_options(cuckoohash PRIVATE /W4 /permissive- /std:c++17)
else()
	target_compile_options(cuckoohash PRIVATE -Wall -pedantic -std=c++17)
endif()


# set_property(TARGET cuckoohash PROPERTY CXX_STANDARD 20)


# TODO: Add tests and install targets if needed.
# add add_executable(tests "test/test_main.cpp" "test/test_cuckoohash.cpp")
# target_include_directories(tests PRIVATE ${CMAKE_LIBRARY_PATH}/include)
# set_property(TARGET tests PROPERTY CXX_STANDARD 20)
# enable_testing()
# add_test(NAME cuckoohash_tests COMMAND tests)

