# CMakeList.txt : CMake project for cuckoohash, include source and define
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable (cuckoohash "main.cpp")

#find_package(libcuckoo REQUIRED)
#find_package(nlohmann_json REQUIRED)
target_include_directories(cuckoohash PRIVATE ${CMAKE_LIBRARY_PATH}/include)

target_link_libraries(cuckoohash PRIVATE libcuckoo -lpthread)
target_link_libraries(cuckoohash PRIVATE nlohmann_json::nlohmann_json)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cuckoohash PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.