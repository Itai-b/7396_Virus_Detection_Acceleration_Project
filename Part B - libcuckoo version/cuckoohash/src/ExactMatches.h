#ifndef _EXACTMATCHES_H
#define _EXACTMATCHES_H

#include "Substring.h"
#include <vector>
#include <string>
#include <iostream>


// ExactMatch class
template<typename T>
class ExactMatch {
public:
	ExactMatch() : rule_id(0), rule_type(""), exact_match(Substring<T>()) {};
	ExactMatch(const ExactMatch& other)
		: rule_id(other.rule_id), rule_type(other.rule_type), exact_match(other.exact_match) {};
	ExactMatch(int rule_id, std::string rule_type, Substring<T> exact_match) 
		: rule_id(rule_id), rule_type(rule_type), exact_match(exact_match) {};
	~ExactMatch() = default;

	int getRuleId() { return rule_id; }
	std::string getRuleType() { return rule_type; }
	Substring<T> getExactMatch() { return exact_match; }
	void printExact
	void setRuleId(int new_id) { rule_id = new_id; }
	void setRuleType(std::string new_type) { rule_type = new_type; }
	void setExactMatch(const Substring<T>& new_exact_match) { exact_match = new_exact_match; }
	
private:
	int rule_id;
	std::string rule_type;
	Substring<T> exact_match;
};


// ExactMatches class
class ExactMatches {
public:
	ExactMatches();
	~ExactMatches();
	void insertExactMatch(const ExactMatch& to_insert);
	void clearExactMatches();

private:
	std::vector<ExactMatch*>* exact_matches;
}; 


ExactMatches::ExactMatches() {
	exact_matches = new std::vector<ExactMatch*>();
}

ExactMatches::~ExactMatches() {
	clearExactMatches();
	delete exact_matches;
}

void insertExactMatch(const ExactMatch& to_insert) {
	ExactMatch* exact_match = new ExactMatch(to_insert);
	exact_matches->push_back(exact_match);
}


void ExactMatches::clearExactMatches() {
	for (auto it = exact_matches->begin(); it != exact_matches->end(); ++it)
	{
		delete* it;
	}
	exact_matches->clear();
}

#endif // _EXACTMATCHES_H