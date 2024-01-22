#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string>
#include <set>
#include <cstdint>
#include <iostream>
#include <iomanip>

#define _SUBSTRING_DEFAULT_GAP 1

/**
 * A string of L=sizeof(T) chars, represented as an unsigned int
 *
 * @tparam T the type of the unsigned int which will represent the substring,
 * the caller must use an unsigned int which correlates to the L chosen,
 * i.e., T = {uint8_t, uint16_t, uint32_t, uint64_t,...} for L = {1, 2, 4, 8,...} respectively.
 */
template <typename T>
class Substring {
public:
	T substring;

	Substring(const T& substring) : substring(substring) {}
	Substring(std::string hexString) { // Assumes hexString represents a string of sizeof(T) hexadecimal bytes ("FF" for the char of ASCII = 255).
		substring = 0;
		size_t len = hexString.size();
		for (size_t i = 0; i < len; i++) {
			// since a char in hexString is only 1 hex, it is half a byte => 4 bits shift only
			substring = (substring << 4) | hexCharToInt(hexString[i]);
		}
	}
	Substring() : substring(0) {}
	~Substring() = default;

	T getSubstring() { return this->substring; }

	void setSubstring(T substring) { this->substring = substring; }

	bool operator<(const Substring<T>& other) const {
		return this->substring < other.substring;
	}

	bool operator>(const Substring<T>& other) const {
		return this->substring > other.substring;
	}

	bool operator>=(const Substring<T>& other) const {
		return *this > other || *this == other;
	}

	bool operator<=(const Substring<T>& other) const {
		return *this < other || *this == other;
	}

	bool operator==(const Substring<T>& other) const {
		return this->substring == other.substring;
	}

	bool operator!=(const Substring<T>& other) const {
		return !(*this == other);
	}

	Substring <T> operator=(const Substring<T>& other) {
		if (this->substring != other.substring) {
			this->substring = other.substring;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Substring& substring) {
		size_t iterations = sizeof(T);
		for (size_t i = iterations; i > 0; --i) {
			uint8_t current_byte = (substring.substring >> ((i - 1) * 8)) & 0xFF;
			os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte);
			//<< std::dec << " ('" << (isprint(current_byte) ? static_cast<char>(current_byte) : "NULL") << "') ";
			os << " ('";
			if (isprint(current_byte)) {
				os << static_cast<char>(current_byte);
			}
			else {
				os << "NPSC"; // Non-Printable or Special Characters
			}
			os << "') ";
		}
		// os << std::hex << substring.substring;
		return os;
	}

	static void extractSubstrings(const std::string& hexString, std::set<Substring<T>>& substrings, \
		size_t G = _SUBSTRING_DEFAULT_GAP, size_t L = sizeof(T)) {
		size_t len = hexString.size();
		for (size_t i = 0; i < len; i += G * 2) {
			std::string substring = hexString.substr(i, L * 2);

			// Pad with zeros if needed
			//substring.insert(substring.begin(), L * 2 - substring.size(), '0'); // TODO: check if padding is in the right place (for 0x01, should it be 0x000001 or 0x010000 ?)

			substrings.insert(Substring<T>(substring));
		}
	}

private:
	static uint8_t hexCharToInt(char hexChar) {
		if (hexChar >= 'a' && hexChar <= 'f') {
			return hexChar - 'a' + 10;
		}
		else if (hexChar >= 'A' && hexChar <= 'F') {
			return hexChar - 'A' + 10;
		}
		else if (hexChar >= '0' && hexChar <= '9') {
			return hexChar - '0';
		}
		return 0; // Invalid hex.
	}
};


#endif // !_SUBSTRING_H
