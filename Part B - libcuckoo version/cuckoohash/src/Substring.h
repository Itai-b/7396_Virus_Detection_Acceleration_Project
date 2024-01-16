#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string>
#include <cstdint>
#include <iostream>


class Substring {
public:
	uint64_t substring;

	Substring(uint64_t substring) : substring(substring) {};
	Substring(std::string s) { // Assumes s is a string of 8 characters or less.
		substring = 0;
		for (int i = 0; i < s.size(); i++) {
			substring = substring << 8;
			substring += s[i];
		}
	}
	~Substring() = default;

	char* getSubstring() {
		char* s = new char[8];
		uint64_t temp = substring;
		for (int i = 7; i >= 0; i--) {
			s[i] = temp & 0xFF;
			temp = temp >> 8;
		}
		return s;
	}

	bool operator==(const Substring& other) {
		return substring == other.substring;
	}

	friend std::ostream& operator<<(std::ostream& os, const Substring& substring) {
		for (int i = 7; i >= 0; --i){
			uint8_t current_byte = (substring.substring >> (i * 8)) & 0xFF;
			os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte) <<
				" ('" << (isprint(current_byte) ? static_cast<char>(current_byte) : 'NULL') << "') ";
			}
			os << std::hex << substring.substring;
			return os;
		}


		for (int i = 7; i >= 0; --i) {
			uint8_t byte = (value >> (i * 8)) & 0xFF;
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte)
				<< " (" << (isprint(byte) ? static_cast<char>(byte) : ' ') << ") ";
		}
		for (size_t i = 0; i < sizeof(uint64_t); ++i) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i])
				<< " (" << (isprint(bytes[i]) ? static_cast<char>(bytes[i]) : ' ') << ") ";
		}

		std::cout << std::endl;
		void setSubstring(char* substring) { this->substring = substring; }
};



#endif // !_SUBSTRING_H
