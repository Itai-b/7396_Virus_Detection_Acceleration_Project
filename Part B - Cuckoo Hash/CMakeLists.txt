cmake_minimum_required(VERSION 3.12)
project(YourProjectName)

# Set the C++ standard to C++11 (or any other version you prefer)
set(CMAKE_CXX_STANDARD 11)

# Link microsoft/Kuku library
find_package(Kuku 2.1 REQUIRED)

# Add our source files
add_executable(kukuEXE test.cpp)

# Link our executable with the Kuku library
target_link_libraries(kukuEXE Kuku::kuku)

