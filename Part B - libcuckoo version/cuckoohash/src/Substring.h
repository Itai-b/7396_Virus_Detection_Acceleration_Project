#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

template <typename T>
class Substring {
public:
	T substring;

	Substring(const T& substring) : substring(substring) {}
	Substring(std::string s) { // Assumes s is a string of size less than or equal to sizeof(T).
		substring = 0;
		for (size_t i = 0; i < s.size(); i++) {
			substring = (substring << 8) | static_cast<T>(s[i]);
		}
	}
	Substring() : substring(0) {}
	~Substring() = default;

	T getSubstring() { return this->substring; }

	void setSubstring(T substring) { this->substring = substring; }

	bool operator==(const Substring<T>& other) const {
		return this->substring == other.substring;
	}

	Substring <T> operator=(const Substring<T>& other) {
		this->substring = other.substring;
		return this->substring;
	}

	friend std::ostream& operator<<(std::ostream& os, const Substring& substring) {
		size_t iterations = sizeof(T);
		for (size_t i = iterations; i > 0; --i) {
			uint8_t current_byte = (substring.substring >> ((i - 1) * 8)) & 0xFF;
			os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte) <<
				" ('" << (isprint(current_byte) ? static_cast<char>(current_byte) : 'NULL') << "') ";
		}
		os << std::hex << substring.substring;
		return os;
	}
};

#endif // !_SUBSTRING_H
