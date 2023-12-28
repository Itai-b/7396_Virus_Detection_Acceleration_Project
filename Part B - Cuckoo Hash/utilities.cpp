#include "utilities.h"


// Converts a string of 8 chars or less to a uint64_t
// Every 8 bits chunk in that 64 bits unsigned integer will represent a char
uint64_t Utilities::stringToUint64(const std::string& input) {
    uint64_t result = 0;

    // Cut 8 chars of the input so it will fit the 64 bits of the uint64_t (64 / 8 = 8 bytes = 8 chars)
    size_t length = std::min(input.length(), static_cast<size_t>(8));

    for (size_t i = 0; i < length; ++i) {
        // Shift to the next 8 bits (= 1 byte)
        result <<= 8;

        // bitwise OR with the current char ('0' OR 'x' = 'x')
        result |= static_cast<uint8_t>(input[i]);
    }

    return result;
}


// Converts an uint64_t into an 8 chars string using the logic mentioned above
std::string Utilities::uint64ToString(uint64_t value) {
    std::string result;

    for (size_t i = 0; i < 8; ++i) {
        // Extract the lowest byte of the input
        uint8_t byte = static_cast<uint8_t>(value);

        // Add the byte to the result string
        result.insert(result.begin(), byte);

        // Shift to the next 8 bits (= 1 byte)
        value >>= 8;
    }

    return result;
}
