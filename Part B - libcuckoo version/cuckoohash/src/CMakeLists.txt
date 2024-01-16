# CMakeList.txt : CMake project for cuckoohash, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (cuckoohash "main.cpp" "ExactMatches.h" "Substring.h")
#target_include_directories(cuckoohash PRIVATE ${CMAKE_SOURCE_DIR}/libcuckoo)
target_include_directories(cuckoohash PRIVATE ${CMAKE_LIBRARY_PATH}/include)

# NECESSARY TO RUN WITH UNIX SCRIPT, REMOVE IF USING VS 2022.
#target_link_libraries(cuckoohash pthread)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cuckoohash PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.