// aho_corasick.cpp : Defines the entry point for the application.
//

#include "aho_corasick.hpp"
#include "Parser.h"
#include "ExactMatches.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

void find(aho_corasick::trie& trie, std::basic_string<char>& text) {
	auto res = trie.parse_text(text);
	
	// DEBUG
	size_t trie_size = trie.traverse_tree();
	std::cout << "TRIE size FINAL: " << trie_size << std::endl;
	// DEBUG

	std::cout << "Parsed [" << res.size() << "] item(s): " << std::endl;
	for (auto match : res) { // res is of class emit
		std::cout << '\t' << match.get_keyword() << std::endl;
	}

	std::cout << std::endl;
}


int main()
{

	// TODO: 
	//		2 insertions ideas:
	//			> Create a huge TRIE for all exactmatches, implement a way to track how many rules per hit or count
	//			> Create a small TRIE for *EACH* **RULE**


	// TOOD: Check with the double insertion ("hers" x2 -> double space?) because it does give double hits(!)
	aho_corasick::trie trie;
	std::cout << "Starting TRIE size: " << trie.traverse_tree() << std::endl;
	std::cout << "Size w/ peripherals: " << trie.traverse_tree(true, true) << std::endl;
	std::basic_string<char> arr[] = {
		{'h', 'e', 'r', 's'},
		{'h', 'e', 'r', 's'},
		{'h', 'e', 'r', 's'},
		{'h', 'e', 'r', 's'},
		{'h', 'e', 'r', 's'},
		{'h', 'e', 'r', 's'},
		{'h', 'i', 's'},
		{'s', 'h', 'e'},
		{'h', 'e'},
		{'\0', '\0', '\0'},
		{'u', '\0'}
	};
	for (auto s : arr) {
		trie.insert(s);
		std::cout << "TRIE size: " << trie.traverse_tree() << ", Size w/ peripherals: " << trie.traverse_tree(true, true) << std::endl;
		std::cout << '\t' << "Number of Keywords: " << trie.getNumKeywords()	<< std::endl;
	}

	
	// Example:
			std::basic_string<char> toParse_spc = { 'u', '\0', '\0', '\0', 's', 'H', 'E', 'r', 's', '\0', '\0' };
			find(trie, toParse_spc);
	// TODO: run valgrind on this sh(!)t, i dont see who deletes after the 'new' allocates by state::add_state().
	return 0;
}
