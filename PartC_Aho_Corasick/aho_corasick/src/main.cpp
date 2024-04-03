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
	std::cout << "TRIE size: " << trie.traverse_tree() << ", Size w/ peripherals: " << trie.traverse_tree(true, true) << std::endl;
	// DEBUG

	std::cout << "Parsed [" << res.size() << "] item(s): " << std::endl;
	for (auto match : res) { // res is of class emit
		std::cout << '\t' << match.get_keyword() << std::endl;
	}

	std::cout << std::endl;
}


int main()
{
	// TOOD: Check with the double insertion ("hers" x2 -> double space?) because it does give double hits(!)
	auto start_time = std::chrono::high_resolution_clock::now();
	std::string file_path = "parta_data_by_exactmatch.json";
	std::string dest_path = "";
	
	ExactMatches exact_matches;
	parseFile(file_path, exact_matches);

	std::vector<bstring> bstrings;
	convertExactMatches(exact_matches, bstrings);
	
	aho_corasick::trie trie;
	std::cout << "Starting TRIE size: " << trie.traverse_tree() << std::endl;
	std::cout << "Size w/ peripherals: " << trie.traverse_tree(true, true) << std::endl;
	
	for (auto s : bstrings) {
		trie.insert(s);
		std::cout << "Inserting: " << s << std::endl;
		std::cout << "TRIE size: " << trie.traverse_tree() << ", Size w/ peripherals: " << trie.traverse_tree(true, true) << std::endl;
		std::cout << '\t' << "Number of Keywords: " << trie.getNumKeywords()	<< std::endl;
	}
	
	// Example:
			std::basic_string<char> toParse_spc = { 'g', 'A', 't', 'e', 'C', 'r', 'a', 'S', 'H', 'E', 'r' };
			find(trie, toParse_spc);
	// TODO: run valgrind on this sh(!)t, i dont see who deletes after the 'new' allocates by state::add_state().
	return 0;
}
