# CMakeList.txt : CMake project for Part B - Cuckoo Hash, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Part B - Cuckoo Hash")

# Add source to this project's executable.
add_executable (CMakeTarget "test.cpp")

# Add subdirectory for LibCuckoo implementation
add_subdirectory(./libcuckoo)

# Include directories for libcuckoo headers
target_include_directories(CMakeTarget PRIVATE libcuckoo)

# Link with libcuckoo library
target_link_libraries(CMakeTarget cuckoohash_map)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.