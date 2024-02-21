#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>

#define SUBSTRING_DEFAULT_GAP 1


/// <summary>
/// A string of L=sizeof(T) chars, represented as an unsigned int
/// </summary>
/// <typeparam name="T">
///	T the type of the unsigned int which will represent the substring, 
/// the caller must use an unsigned int which correlates to the L chosen,
/// i.e., T = { uint8_t, uint16_t, uint32_t, uint64_t,... } for L = { 1, 2, 4, 8,... } respectively.
/// </typeparam>
template <typename T>
class Substring {
public:
	T substring;
	std::set<int>* rules;

	Substring(const std::string hexString, const std::set<int>& rules);
	Substring(const T& substring, const std::set<int>& rules);
	Substring(const Substring<T>& other);
	Substring();
	~Substring();

	T getSubstring() { return this->substring; }
	void setSubstring(T substring) { this->substring = substring; }
	std::size_t getNumOfDups() { return this->num_of_dups; }
	void logDuplicate() { this->num_of_dups++; }
	
	Substring<T>& operator=(const Substring<T>& other);
	bool operator<(const Substring<T>& other) const;
	bool operator>(const Substring<T>& other) const;
	bool operator>=(const Substring<T>& other) const;
	bool operator<=(const Substring<T>& other) const;
	bool operator==(const Substring<T>& other) const;
	bool operator!=(const Substring<T>& other) const;

	std::string toStringHex();
	std::string str();
	std::string toStringFull();
	template<class S> friend std::ostream& operator<<(std::ostream& os, const Substring<S>& substring);

	static void extractSubstrings(const std::string& hexString, std::vector<Substring<T>>& substrings,
		const std::set<int>& rules, std::size_t G = SUBSTRING_DEFAULT_GAP, std::size_t L = sizeof(T));
	
private:
	std::size_t num_of_dups;
	static uint8_t hexCharToInt(const char hexChar);
};


/// <summary>
/// Main c'tor of substring. Creates an element of class Substring using a hexString (WITHOUT '0x).
/// Assumes hexString represents a string of sizeof(T) or less bytes represented in hexadecimal.
/// For example: for the word "snort" the equivalent hexString is "736E6F7274", since:
///		0x73 is a byte representing 's'
///		0x6e is a byte representing 'n'
///		0x6f is a byte representing 'o'
///		0x72 is a byte representing 'r'
///		0x74 is a byte representing 't'
/// It is 5 bytes of data, so the appropriate T would be uint64_t.
/// The respective T substring would be: 0b0111001101101110011011110111001001110100 = 8581144067694250176.
/// </summary>
/// <param name="hexString">A string of sizeof(T) or less bytes represented in hex (for example: "0x736E6F7274" for "snort").</param>
/// <param name="rules">A set of integers representing the rule nubmers that match this substring.</param>
template<typename T>
Substring<T>::Substring(const std::string hexString, const std::set<int>& rules) : num_of_dups(0) {
	this->substring = 0;
	std::size_t len = hexString.size();
	for (std::size_t i = 0; i < len; i++) {
		// since a char in hexString is only 1 hex, it is half a byte => 4 bits shift only
		this->substring = (this->substring << 4) | hexCharToInt(hexString[i]);
	}
	this->rules = new std::set<int>(rules);
}

template<typename T>
Substring<T>::Substring(const T& substring, const std::set<int>& rules) : substring(substring), num_of_dups(0) {
	this->rules = new std::set<int>(rules);
}


/// <summary>
/// Copy C'tor. Must have for correct copying of substrings to a set / a vector (deep copy and not ptr value only).
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="other"></param>
template<typename T>
Substring<T>::Substring(const Substring<T>& other) : substring(other.substring), num_of_dups(other.num_of_dups) {
	this->rules = new std::set<int>(*other.rules);
}


// use for debug and testing, remove before release
template<typename T>
Substring<T>::Substring() : substring(0), num_of_dups(0) {
	this->rules = new std::set<int>();
}

template<typename T>
Substring<T>::~Substring() {
	delete this->rules;
	this->rules = nullptr;
}

template<typename T>
Substring<T>& Substring<T>::operator=(const Substring<T>& other) {
	if (this != &other) {
		this->substring = other.substring;
		this->num_of_dups = other.num_of_dups;
		if (this->rules != nullptr) {
			delete this->rules;
		}
		this->rules = new std::set<int>(*other.rules);
	}
	return *this;
}

/* template<typename T>
Substring<T>& Substring<T>::operator=(const Substring<T>& other) {
	if (this->substring != other.substring) {
		this->substring = other.substring;
		this->rules->clear();	// clearing rules only if they were different
	}							// otherwise we want to append the added rules
	this->rules->insert(other.rules->begin(), other.rules->end());
	return *this;
} */

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
std::string Substring<T>::toStringHex() {
	std::ostringstream oss;
	std::size_t iterations = sizeof(T);
	oss << "0x";
	for (std::size_t i = iterations; i > 0; --i) {
		uint8_t current_byte = (this->substring >> ((i - 1) * 8)) & 0xFF;
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte);
	}
	return oss.str();
}

template<typename T>
std::string Substring<T>::str() {
	std::ostringstream oss;
	std::size_t iterations = sizeof(T);
	for (std::size_t i = iterations; i > 0; --i) {
		uint8_t current_byte = (this->substring >> ((i - 1) * 8)) & 0xFF;
		if (std::isprint(current_byte)) {
			oss << static_cast<char>(current_byte);
		}
		else {
			oss << "?"; // Joker used to represent Non-Printable or Special Characters
		}
	}
	return oss.str();
}

template<typename T>
std::string Substring<T>::toStringFull() {
	std::ostringstream oss;
	std::size_t iterations = sizeof(T);
	for (std::size_t i = iterations; i > 0; --i) {
		uint8_t current_byte = (this->substring >> ((i - 1) * 8)) & 0xFF;
		oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(current_byte) << " ('";
		if (std::isprint(current_byte)) {
			oss << static_cast<char>(current_byte) << "') ";
		}
		else {
			oss << "NPSC" << "') "; // Non-Printable or Special Characters
		}
	}
	return oss.str();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Substring<T>& substring) {
	os << substring.toStringFull();
	return os;
}

/// <summary>
/// Given a hexString and a set of Substrings to fill, this function will remove the "0x" from the hexString
/// and will fill the Substrings set with cutted chunks of length L from the original hexString,
/// skipping G chars every time. If the substring is a duplicate, it will only take its rules to the already existing one.
/// Example: hexString = "0x736E6F7274", with L = 2 (= 2 bytes = 2 chars = 4 hex), G = 1:
///		Substring([736E, 6E6F, 6F72, 7274]) will be created. 
/// Notes: > This function does *NOT* zero pad. If hexString.size() < L it won't insert it.
///		   > If there are 2 equal substrings with *different* rules, only 1 substring will be added with the *combined* set of rules. 	
/// </summary>
/// <typeparam name="T">Type of the substring to be created (uint16_t, uint32_t, uint64_t, ...)</typeparam>
/// <param name="hexString">The hexString to be processed and split ("0x736E6F7274") </param>
/// <param name="substrings">The set of Substrings in which the results will be stored</param>
/// <param name="G">The gap in which the parser advances on the original string (default: G = 1)</param>
/// <param name="L">The length of each splitted substring (default: L = sizeof(T))</param>
template <typename T>
void Substring<T>::extractSubstrings(const std::string& hexString, std::vector<Substring<T>>& substrings, 
									 const std::set<int>& rules, std::size_t G, std::size_t L) {
	// Defaults for G and L are provided in function's declaration.
	std::string hexOnlyStr = (hexString.substr(0, 2) == "0x") ? hexString.substr(2) : hexString;
	std::size_t len = hexOnlyStr.size();
	// Jumping by 2*L and 2*G because in hex representation each char = 2 hexDigits.
	for (std::size_t i = 0; (i < len) && ((i + L * 2) <= len); i += G * 2) {
		std::string hexSubstring = hexOnlyStr.substr(i, L * 2);
		Substring<T> substring(hexSubstring, rules);
		auto it = std::find(substrings.begin(), substrings.end(), substring);
		// check if an equivalent substring is already in the vector
		if (it != substrings.end()) {	// substring found in vector - it will now represent the combined set of rules.
			it->rules->insert(rules.begin(), rules.end());
			it->logDuplicate();
		}
		else {							// substring is not in vector - add substring to vector
			substrings.push_back(substring);
		}
	}
}

template <typename T>
uint8_t Substring<T>::hexCharToInt(const char hexChar) {
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

#endif // _SUBSTRING_H
