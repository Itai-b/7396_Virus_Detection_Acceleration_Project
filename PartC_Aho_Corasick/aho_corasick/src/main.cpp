// aho_corasick.cpp : Defines the entry point for the application.
//

#include <aho_corasick/aho_corasick.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>


using namespace std;

int main()
{
	aho_corasick::trie trie;
	string arr[] = {"hers", "his", "she", "he"};
	for (auto s : arr) {
		trie.insert(s);
	}
	auto res = trie.parse_text("ushers");
	return 0;
}
