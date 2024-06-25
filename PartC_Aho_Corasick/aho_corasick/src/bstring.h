#ifndef _BSTRING_DEFINITIONS_H_
#define _BSTRING_DEFINITIONS_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

typedef std::basic_string<char> bstring;

void hexToBstring(const std::string& hexString, bstring& bstring) {
    // Remove the "0x" prefix if present
    std::string cleanedHexString = hexString.substr(0, 2) == "0x" ? hexString.substr(2) : hexString;

    // Ensure the input string has an even number of characters
    std::size_t len = cleanedHexString.size();
    if (len % 2 != 0) {
        throw std::invalid_argument("Hex string length must be even!");
    }

    // Convert each pair of hexadecimal characters to a byte
    for (size_t i = 0; i < len; i += 2) {
        std::istringstream iss(cleanedHexString.substr(i, 2));
        int byteValue;
        iss >> std::hex >> byteValue;
        bstring.push_back(static_cast<char>(byteValue));
    }
}

#endif //_BSTRING_DEFINITIONS_H_