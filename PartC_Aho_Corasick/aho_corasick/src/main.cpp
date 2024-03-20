// aho_corasick.cpp : Defines the entry point for the application.
//

#include "aho_corasick.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>


using namespace std;


// TODO: implement function to actually return approx. size of the trie tree.
void print_tree_size(const aho_corasick::trie& trie) {
	std::size_t tree_size = sizeof(trie);
	std::cout << "Tree Size: " << tree_size << std::endl;
}

int main()
{

	// TODO: 
	//		2 insertions ideas:
	//			> Create a huge TRIE for all exactmatches, implement a way to track how many rules per hit or count
	//			> Create a small TRIE for *EACH* **RULE**


	// TOOD: Check with the double insertion ("hers" x2 -> double space?) because it does give double hits(!)
	aho_corasick::trie trie;
	print_tree_size(trie);
	basic_string<char> arr[] = {
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
		print_tree_size(trie);
		std::cout << '\t' << "Number of Keywords: " << trie.getNumKeywords()	<< std::endl;
		std::cout << '\t' << "Number of States: "	<< trie.getNumStates()		<< std::endl;
		std::cout << '\t' << "Size of a State: "	<< trie.getStateSize()		<< std::endl;
		std::cout << '\t' << "Size of TRIE: "		<< trie.size()				<< std::endl;
	}

	string toParse_str = "u\0\0\0sHErs\0\0";
	std::cout << "str len is: " << toParse_str.length() << std::endl;

	basic_string<char> toParse_spc = { 'u', '\0', '\0', '\0', 's', 'H', 'E', 'r', 's', '\0', '\0' };
	std::cout << "spc len is: " << toParse_spc.length() << std::endl;

	auto res = trie.parse_text(toParse_spc);
	print_tree_size(trie);

	// TODO: fix bug where text is being parsed until the first '\0'.
	std::cout << "Parsed [" << res.size() << "] item(s): " << std::endl;
	for (auto match : res) { // res if of class emit
		std::cout << '\t' << match.get_keyword() << std::endl;
	}


	// TODO: run valgrind on this sh(!)t, i dont see who deletes after the 'new' allocates by state::add_state().
	return 0;
}
