#ifndef _EXACTMATCHES_H
#define _EXACTMATCHES_H

#include "bstring.h"
#include <set>
#include <string>
#include <iostream>

// ExactMatch class
class ExactMatch {
public:
	ExactMatch() : exact_match("") {};
	ExactMatch(const ExactMatch& other)
		: rules_number(other.rules_number), exact_match(other.exact_match) {};
	ExactMatch(std::set<int> rules_number, std::string exact_match)
		: rules_number(rules_number), exact_match(exact_match) {};
	~ExactMatch() = default;

	std::set<int> getRulesNumbers() { return rules_number; }
	std::string getExactMatch() { return exact_match; }
	void setExactMatch(const std::string& new_exact_match) { exact_match = new_exact_match; }
	
private:
	std::set<int> rules_number;
	std::string exact_match;
};


// ExactMatches class
class ExactMatches {
public:
	ExactMatches();
	~ExactMatches();
	void insert(const ExactMatch& to_insert);
	void clear();
	void createMap(std::map<bstring, std::set<int>>& map);
	std::vector<ExactMatch*>* exact_matches;
}; 


ExactMatches::ExactMatches() {
	exact_matches = new std::vector<ExactMatch*>();
}

ExactMatches::~ExactMatches() {
	clear();
	delete exact_matches;
}

void ExactMatches::insert(const ExactMatch& to_insert) {
	ExactMatch* exact_match = new ExactMatch(to_insert);
	exact_matches->push_back(exact_match);
}


void ExactMatches::clear() {
	for (auto it = exact_matches->begin(); it != exact_matches->end(); ++it)
	{
		delete* it;
	}
}

void ExactMatches::createMap(std::map<std::basic_string<char>, std::set<int>>& map) {
	for (ExactMatch* match : *exact_matches) {
		bstring bstr;
		hexToBstring(match->getExactMatch(), bstr);
		map[bstr] = match->getRulesNumbers();
	}
} 

#endif // _EXACTMATCHES_H