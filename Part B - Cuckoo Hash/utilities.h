#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstdint>
#include <string>

class Utilities {
public:
    static uint64_t stringToUint64(const std::string& input);
    static std::string uint64ToString(uint64_t value);
};

#endif // UTILITIES_H
