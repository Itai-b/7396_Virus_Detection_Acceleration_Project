#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string>
#include <set>
#include <cstdint>
#include <iostream>
#include <iomanip>

#define SUBSTRING_DEFAULT_GAP 1

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

	Substring(std::string hexString);
	Substring(const T& substring) : substring(substring) {}
	Substring() : substring(0) {}
	~Substring() = default;

	T getSubstring() { return this->substring; }
	void setSubstring(T substring) { this->substring = substring; }
	
	Substring<T> operator=(const Substring<T>& other);
	bool operator<(const Substring<T>& other) const;
	bool operator>(const Substring<T>& other) const;
	bool operator>=(const Substring<T>& other) const;
	bool operator<=(const Substring<T>& other) const;
	bool operator==(const Substring<T>& other) const;
	bool operator!=(const Substring<T>& other) const;

	template<class S> friend std::ostream& operator<<(std::ostream& os, const Substring<S>& substring);

	static void extractSubstrings(const std::string& hexString, std::set<Substring<T>>& substrings, \
		std::size_t G = SUBSTRING_DEFAULT_GAP, std::size_t L = sizeof(T));

private:
	static uint8_t hexCharToInt(char hexChar);
};


/// <summary>
/// Main c'tor of substring. Creates an element of class Substring using a hexString.
/// Assumes hexString represents a string of sizeof(T) or less bytes represented in hexadecimal.
/// For example: for the word "snort" the equivalent hexString is "0x736E6F7274", since:
///		0x73 is a byte representing 's'
///		0x6e is a byte representing 'n'
///		0x6f is a byte representing 'o'
///		0x72 is a byte representing 'r'
///		0x74 is a byte representing 't'
/// It is 5 bytes of data, so the appropriate T would be uint64_t.
/// The respective T substring would be: 0b0111001101101110011011110111001001110100 = 8581144067694250176.
/// </summary>
/// <param name="hexString">A string of sizeof(T) or less bytes represented in hex (for example: "0x736E6F7274" for "snort").</param>
template<typename T>
Substring<T>::Substring(std::string hexString) {
	substring = 0;
	std::size_t len = hexString.size();
	for (std::size_t i = 0; i < len; i++) {
		// since a char in hexString is only 1 hex, it is half a byte => 4 bits shift only
		substring = (substring << 4) | hexCharToInt(hexString[i]);
	}
}

template<typename T>
bool Substring<T>::operator<(const Substring<T>& other) const {
	return this->substring < other.substring;
}

template<typename T>
bool Substring<T>::operator>(const Substring<T>& other) const {
	return this->substring > other.substring;
}

template<typename T>
bool Substring<T>::operator>=(const Substring<T>& other) const {
	return *this > other || *this == other;
}

template<typename T>
bool Substring<T>::operator<=(const Substring<T>& other) const {
	return *this < other || *this == other;
}

template<typename T>
bool Substring<T>::operator==(const Substring<T>& other) const {
	return this->substring == other.substring;
}

template<typename T>
bool Substring<T>::operator!=(const Substring<T>& other) const {
	return !(*this == other);
}

template<typename T>
Substring<T> Substring<T>::operator=(const Substring<T>& other) {
	if (this->substring != other.substring) {
		this->substring = other.substring;
	}
	return *this;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Substring<T>& substring) {
	std::size_t iterations = sizeof(T);
	for (std::size_t i = iterations; i > 0; --i) {
		uint8_t current_byte = (substring.substring >> ((i - 1) * 8)) & 0xFF;
		os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte) << " ('";
		if (isprint(current_byte)) {
			os << static_cast<char>(current_byte) << "') ";
		}
		else {
			os << "NPSC" << "') "; // Non-Printable or Special Characters
		}
	}
	// DEBUG
	// os << std::hex << substring.substring << std::endl;
	return os;
}

template <typename T>
void Substring<T>::extractSubstrings(const std::string& hexString, std::set<Substring<T>>& substrings, std::size_t G, std::size_t L) {
	// Defaults for G and L are provided in function's declaration.
	std::size_t len = hexString.size();
	for (std::size_t i = 0; (i < len) && (i + L*2 <= len); i += G * 2) {
		std::string hexSubstring = hexString.substr(i, L * 2);
		substrings.insert(Substring<T>(hexSubstring));	// c'tor of Substring<T> creates a substring (represted as typename T) from string hexSubstring.
	}
}

template <typename T>
uint8_t Substring<T>::hexCharToInt(char hexChar) {
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

#endif // !_SUBSTRING_H
