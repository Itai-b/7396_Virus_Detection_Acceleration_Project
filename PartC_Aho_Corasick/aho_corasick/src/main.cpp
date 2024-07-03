#include "Auxiliary.h"

// Theoretically we count implement the AC automaton using a transition table and a go-to table for each state.
// In this minimalistic implementation, the total size would be the number of edges times 11, since:
// for each edge, we would keep the following information: { Curr_State [3Bytes], Curr_Char [1Byte], Next_State [3Bytes], IBLT_PTR [4Bytes] }.
// (This is assuming a x32-bits hardware).
#define SIZE_OF_GO_TO_TABLE_ENTRY 11	// Bytes

// Theoretical calculations for the addition size needed to store the rules' SID(s) list / IBLT for each entry
const std::size_t SID_ENTRY_IN_LINKED_LIST = 64; // size in bits (32bits for the SID, 32bits for the pointer to next item)
const std::size_t IBLT_CELL_SIZE = 40; // size in bits (32 bits for the SID xor sum, 8 bits for the Bloom Filter)
const std::size_t IBLT_CELLS_SUCCESS_RATE_100 = 256; // based on part D
const std::size_t IBLT_CELLS_SUCCESS_RATE_99 = 32; // based on part D
const std::size_t IBLT_CELLS_SUCCESS_RATE_95 = 8; // based on part D


/// <summary>
/// Parse text to find exact matches in the Aho Corasick TRIE.
/// Prints any exact matches found.
/// </summary>
/// <param name="trie">The Aho Corasick State Machine (TRIE tree)</param>
/// <param name="text">The input text to parse</param>
void find(aho_corasick::trie* trie, bstring& text) {
	auto res = trie->parse_text(text);
	std::cout << "Parsed [" << res.size() << "] item(s): " << std::endl;
	for (auto match : res) { // res is of class emit
		std::cout << '\t' << match.get_keyword() << std::endl;
	}
	std::cout << std::endl;
}

/// <summary>
/// Parse text to find exact matches in the Aho Corasick TRIE.
/// Prints any exact matches found.
/// </summary>
/// <param name="trie">The Aho Corasick State Machine (TRIE tree)</param>
/// <param name="text">The input text to parse</param>
/// <param name="log">The respective SearchResults item where the hits would be registered</param>
void find(aho_corasick::trie* trie, bstring& text, SearchResults& log, const std::map<bstring, std::set<int>>& map) {
	auto res = trie->parse_text(text);
	//std::cout << "Matched on " << res.size() << " item(s)" << std::endl;
	for (auto test : res) { // res is of class emit
		//std::cout << '\t' << test.get_keyword() << std::endl;
		bstring test_bstring = test.get_keyword();
		try {
			const std::set<int>& rules = map.at(test_bstring);
			for (int rule : rules) {
				log.sids_hit[rule]++;
			}
		}
		catch (const std::out_of_range& e) {
			// Handle case where key is not found
			std::cout << "\tNo rules were found for this test." << std::endl;
		}
	}
	for (auto original_sid : log.original_sids) {
				std::cout << "SID:" << original_sid << " was hit "  << log.sids_hit[original_sid] << " times." << std::endl;
	}
	std::cout << std::endl;
}


/// <summary>
/// Run a single test of insertion to the Aho Corasick TRIE tree for a given min threshold on the exact matches.
/// (no need to run more than 1 test because TRIE Theorm ensures that for every random order insertion the TRIE will look the same.
/// </summary>
/// <param name="stats">A class member of Statistics</param>
/// <param name="threshold">Minimum length threshold for the exact matches (take only exact matches with length >= threshold)</param>
/// <param name="bstrings">An std::vector of the basic_string<char> represeting the exact matches to insert</param>
void runTest(Statistics& stats, Results& results, const size_t threshold, const std::vector<bstring>& bstrings, 
	std::vector<SearchResults>* search_results, std::map<bstring, std::set<int>>& sids_map) {
	for (auto it = (*search_results).begin(); it != (*search_results).end(); it++) {
		it->sids_hit.clear();
	}
	// Pre test: setting a vector of the min thresholded basic_strings:
	std::vector<bstring> thresholded_bstrings;
	for (auto s : bstrings) {
		if (s.length() >= threshold) {
			thresholded_bstrings.push_back(s);
		}
	}

	// Remove all entries of the (pattern, sids) map, that have patterns of length below threshold
	for (auto it = sids_map.begin(); it != sids_map.end(); ) {
		if (it->first.length() < threshold) {
			it = sids_map.erase(it);
		}
		else {
			++it;
		}
	}
	
	std::vector<bstring> search_strings;
	toBstring(search_results, search_strings);

	aho_corasick::trie* aho_corasick_trie;
	std::size_t nodes_size = 0;
	std::size_t total_edges = 0;
	std::size_t size_in_theory = 0;
	std::size_t aho_corasick_size = 0;
	std::size_t aho_corasick_no_emits_size = 0;
	std::size_t exact_matches_inserted = 0;

	// TIME STAMP BEGIN: initiate Aho Corasick state machine
	auto timestamp_a = std::chrono::high_resolution_clock::now();

	// Allocate a new TRIE tree for test timing consistancy
	aho_corasick_trie = new aho_corasick::trie();

	// Insert the exact matches into the aho corasick TRIE
	for (bstring s : thresholded_bstrings) {
		aho_corasick_trie->insert(s);
		exact_matches_inserted++;
	}

	nodes_size = aho_corasick_trie->traverse_tree();
	total_edges = aho_corasick_trie->count_edges();
	size_in_theory = total_edges * SIZE_OF_GO_TO_TABLE_ENTRY;
	aho_corasick_size = aho_corasick_trie->traverse_tree(true, true);
	aho_corasick_no_emits_size = aho_corasick_trie->traverse_tree(true, false);

	if (threshold >= 1 && threshold <= 8){ // Relevant range for searching the tree
		int i = 0;
		std::cout << "==================================== Building Aho_Corasick with Threshold <= " << threshold << " Bytes ==========================================" << std::endl;
		for (bstring& search_string : search_strings) {
			std::cout << "Test #" << i+1 << " results:" << std::endl;
			find(aho_corasick_trie, search_string, (*search_results)[i], sids_map);
			//results.addData((*search_results)[i]);
			++i;
		}
	}

	delete aho_corasick_trie;

	// TIME STAMP END: delete aho corasick automaton
	auto timestamp_b = std::chrono::high_resolution_clock::now();
	auto test_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_b - timestamp_a).count();

	// Collect and Print Statistics:
	TestStatistics test_data = {
		nodes_size,
		total_edges,
		size_in_theory,
		aho_corasick_size,
		aho_corasick_no_emits_size,
		exact_matches_inserted,
		threshold,
		static_cast<double>(test_runtime)
	};
	stats.addData(test_data);

	if (threshold >= 1 && threshold <= 8) {
		std::cout << "Statistics for Aho_Corasick with Threshold <= " << threshold;
		std::cout << std::endl << std::dec << exact_matches_inserted << " Exact Match(es) were inserted." << std::endl		\
			<< "Aho Corasick size: " << size_in_theory << " Bytes" << std::endl												\
			<< "Insertion time: " << static_cast<double>(test_runtime) << "[ms]." << std::endl								\
			<< std::endl;
	}
}
	

/// <summary>
/// Parse the .json file, which was generated by the python script in Part A, for ExactMatches.
/// Each ExactMatch includes the extracted sub-exact match from a given rule, the rule type (content / pcre) and relevant line number in the snort file.
/// 1) Parse the file to extract lines
//      line example:
//      //std::string line = "[12, \"pcre\", [[\"0x00\", \"0x00\", \"0x00\", \"0x65\", \"0x63\", \"0x72\", \"0x61\", \"0x73\", \"0x68\", \"0x65\", \"0x00\"], [\"0x73\", \"0x65\", \"0x72\", \"0x76\", \"0x65\", \"0x72\"], [\"0x6f\", \"0x6e\", \"0x2d\", \"0x6c\", \"0x69\", \"0x6e\", \"0x65\", \"0x2e\", \"0x2e\", \"0x2e\"]]]";
// 2) From each line (/rule) extract exact matches
// 3) For each desired L = {2, 4, 8}:
//      A) For each desired G = {1, 2, 4, L}:
//      B) Make 100 trials of inserting RANDOMALLY to the cuckoohash and check throughput
// 4) Store data and plot graphs
/// </summary>
/// <param name="argc"></param>
/// <param name="argv">Run with path to exact_matches_hex.json from terminal/script.</param>
/// <returns></returns>
int main(int argc, char* argv[]) {
	auto start_time = std::chrono::high_resolution_clock::now();
	std::string file_path = "parta_data_by_exactmatch.json";
	std::string dest_path = "";
	std::string test_path = "snort_string_check.json";
	
	// Retrieving arguments from WSL / Visual Studio
	getOpts(argc, argv, file_path, dest_path, test_path);

	// Parsing input file of patterns to add to the Aho Corasick TRIE
	ExactMatches exact_matches;
	parseFile(file_path, exact_matches);

	// Creating a map for each input exact match with its respective rule
	std::map<bstring, std::set<int>> sids_map;
	exact_matches.createMap(sids_map);

	// Parsing test file of patterns to search the Aho Corasick TRIE
	std::vector<SearchResults> search_results;
	parseFile(test_path, search_results);

	// Setting max threshold as the longest bstring length + 1
	std::vector<bstring> bstrings;
	std::size_t max_length = toBstring(exact_matches, bstrings);
	std::size_t max_threshold = max_length + 1;

	// Running tests: Aho Corasick TRIE creation, insertion and search
	Statistics stats;
	for (std::size_t threshold = 1; threshold <= max_threshold; ++threshold) {
		Results results;
		runTest(stats, results, threshold, bstrings, &search_results, sids_map);
		std::string res_file_name = "search_results_threshold_" + std::to_string(threshold) + ".json";

		// additional storage calculation
		std::size_t raw_list_size = 0;
		std::size_t iblt_size_optimal = 0;
		std::size_t iblt_size_100_rate = 0;
		std::size_t iblt_size_99_rate = 0;
		std::size_t iblt_size_95_rate = 0;
		
		// In order to have comparison ground with the Hash Table, we will also check the search results (hits/misses) using threshold
		if (threshold >= 1 && threshold <= 8) {
			// Calculate additional size required
			for (ExactMatch* exact_match : (*exact_matches.exact_matches)) {
				bstring bstr;
				hexToBstring(exact_match->getExactMatch(), bstr);
				//std::cout << "Exact Match: " << bstr << std::endl;
				//std::cout << "Exact Match Length: " << bstr.length() << std::endl;
				if (bstr.length() >= threshold) {	// Exact Match is in the Aho Corasick for current threshold
					raw_list_size += exact_match->getRulesNumbers().size() * SID_ENTRY_IN_LINKED_LIST;
					iblt_size_optimal += 2 * exact_match->getRulesNumbers().size() * IBLT_CELL_SIZE;
					iblt_size_100_rate += IBLT_CELLS_SUCCESS_RATE_100 * IBLT_CELL_SIZE;
					iblt_size_99_rate += IBLT_CELLS_SUCCESS_RATE_99 * IBLT_CELL_SIZE;
					iblt_size_95_rate += IBLT_CELLS_SUCCESS_RATE_95 * IBLT_CELL_SIZE;
				}
			}

			for (SearchResults search_item : search_results) {
				search_item.full_list_size = int(raw_list_size / 8);
				search_item.iblt_size_optimal = int(iblt_size_optimal / 8);
				search_item.iblt_size_100_rate = int(iblt_size_100_rate / 8);
				search_item.iblt_size_99_rate = int(iblt_size_99_rate / 8);
				search_item.iblt_size_95_rate = int(iblt_size_95_rate / 8);
				results.addData(search_item);
			}

			results.writeToFile(dest_path, res_file_name);
			// Clear sids_hit histogram for next threshold search
			for (SearchResults search_item : search_results) {
				search_item.sids_hit.clear();
			}
		}

	}
	stats.writeToFile(dest_path, "partc_results.json");

	auto end_time = std::chrono::high_resolution_clock::now();
	auto total_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Finished running Aho Corasick Tests in " << static_cast<double>(total_runtime) << "[ms]." << std::endl;
	return 0;
}
