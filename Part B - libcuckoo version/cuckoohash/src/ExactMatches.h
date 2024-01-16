#ifndef _EXACTMATCHES_H
#define _EXACTMATCHES_H

#include <vector>
#include <string>
#include <iostream>


// ExactMatch class
class ExactMatch {
public:
	ExactMatch() : rule_id(0), rule_type(""), exact_match(nullptr) {};
	ExactMatch(int rule_id, std::string rule_type, char* exact_match);
	~ExactMatch() { delete exact_match; };

	int getRuleId() { return rule_id; }
	std::string getRuleType() { return rule_type; }
	char* getExactMatch() { return exact_match; }
	void setRuleId(int id) { rule_id = id; }
	void setRuleType(std::string type) { rule_type = type; }
	
private:
	int rule_id;
	std::string rule_type;
	char* exact_match; // TODO: change to class Substring after implementing it.
};

ExactMatch::ExactMatch(int rule_id, std::string rule_type, char* exact_match) : rule_id(rule_id), rule_type(rule_type) {
	this->exact_match = new char(sizeof(exact_match));
	for(int i = 0; i < sizeof(exact_match); i++) {
		this->exact_match[i] = exact_match[i];
	}
}


// ExactMatches class
class ExactMatches {
public:
	ExactMatches();
	~ExactMatches();
	void insertExactMatch(ExactMatch* exact_match) { exact_matches->push_back(exact_match); }
	void clearExactMatches();

private:
	std::vector<ExactMatch*>* exact_matches;
}; 

ExactMatches::ExactMatches()
{
	exact_matches = new std::vector<ExactMatch*>();
}

ExactMatches::~ExactMatches()
{
	clearExactMatches();
	delete exact_matches;
}

void ExactMatches::clearExactMatches()
{
	for (auto it = exact_matches->begin(); it != exact_matches->end(); ++it)
	{
		delete* it;
	}
	exact_matches->clear();
}

#endif // _EXACTMATCHES_H