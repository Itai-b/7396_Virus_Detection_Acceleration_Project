#ifndef _EXACTMATCHES_H
#define _EXACTMATCHES_H

#include <vector>
#include <string>
#include <iostream>


enum SignatureType {CONTENT = 1, PCRE = 2, OTHER = 0};

// ExactMatch class
class ExactMatch {
public:
	ExactMatch() : signature_type(OTHER), exact_match("") {};
	ExactMatch(const ExactMatch& other)
		: rules_number(other.rules_number), signature_type(other.signature_type), exact_match(other.exact_match) {};
	ExactMatch(std::vector<int> rules_number, std::string signature_type, std::string exact_match)
		: rules_number(rules_number), exact_match(exact_match) {
		setSignatureType(signature_type);
	};
	ExactMatch(std::vector<int> rules_number, SignatureType signature_type, std::string exact_match)
		: rules_number(rules_number), signature_type(signature_type), exact_match(exact_match) {};
	~ExactMatch() = default;

	std::vector<int> getRulesNumbers() { return rules_number; }
	std::string getSignatureType() { if (signature_type == CONTENT) { return "content"; } else if (signature_type == PCRE) { return "pcre"; } else { return "other"; } }
	std::string getExactMatch() { return exact_match; }
	void setSignatureType(std::string new_type) { if (new_type == "content") { signature_type = CONTENT; } else if (new_type == "pcre") { signature_type = PCRE; } else { signature_type = OTHER; } }
	void setExactMatch(const std::string& new_exact_match) { exact_match = new_exact_match; }
	
private:
	std::vector<int> rules_number;
	SignatureType signature_type;
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