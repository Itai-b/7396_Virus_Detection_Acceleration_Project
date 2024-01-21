#ifndef _EXACTMATCHES_H
#define _EXACTMATCHES_H

#include "Substring.h"
#include <vector>
#include <string>
#include <iostream>


typedef enum RuleType {CONTENT = 1, PCRE = 2, OTHER = 0} rule_type;

// ExactMatch class
class ExactMatch {
public:
	ExactMatch() : rule_id(0), rule_type(OTHER), exact_match("") {};
	ExactMatch(const ExactMatch& other)
		: rule_id(other.rule_id), rule_type(other.rule_type), exact_match(other.exact_match) {};
	ExactMatch(int rule_id, std::string rule_type, std::string exact_match)
		: rule_id(rule_id), exact_match(exact_match) {
		setRuleType(rule_type);
	};
	ExactMatch(int rule_id, rule_type rule_type, std::string exact_match) 
		: rule_id(rule_id), rule_type(rule_type), exact_match(exact_match) {};
	~ExactMatch() = default;

	int getRuleId() { return rule_id; }
	std::string getRuleType() { if (rule_type == CONTENT) { return "content"; } else if (rule_type == PCRE) { return "pcre"; } else { return "other"; } }
	std::string getExactMatch() { return exact_match; }
	void setRuleId(int new_id) { rule_id = new_id; }
	void setRuleType(std::string new_type) { if (new_type == "content") { rule_type = CONTENT; } else if (new_type == "pcre") { rule_type = PCRE; } else { rule_type = OTHER; } }
	void setExactMatch(const std::string& new_exact_match) { exact_match = new_exact_match; }
	
private:
	int rule_id;
	rule_type rule_type;
	std::string exact_match;
};


// ExactMatches class
class ExactMatches {
public:
	ExactMatches();
	~ExactMatches();
	void insert(const ExactMatch& to_insert);
	void clear();
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

#endif // _EXACTMATCHES_H