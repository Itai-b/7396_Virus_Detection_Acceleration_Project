/*
* Copyright (C) 2018 Christopher Gilbert.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

// Revised by Idan Baruch & Itai Benyamin. Up to date: 03/04/24.

#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include <algorithm>
#include <cctype>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <queue>
#include <utility>
#include <vector>
#include <mutex>
#include <atomic>


namespace aho_corasick {

// Default configurations for the behavior of the aho-corasick trie tree.
// Change using trie.config...
const bool DEFAULT_OVERLAPS = true;
const bool DEFAULT_WHOLE_WORDS = false;
const bool DEFAULT_INSENSITIVE = true;

	/// <summary>
	///		Class Interval represents an interval of [d_start, d_end].
	/// </summary>
	class interval {
		size_t d_start;
		size_t d_end;

	public:
		interval(size_t start, size_t end)
			: d_start(start)
			, d_end(end) {}

		size_t get_start() const { return d_start; }
		size_t get_end() const { return d_end; }
		size_t size() const { return d_end - d_start + 1; }

		bool overlaps_with(const interval& other) const {
			return d_start <= other.d_end && d_end >= other.d_start;
		}

		bool overlaps_with(size_t point) const {
			return d_start <= point && point <= d_end;
		}

		bool operator <(const interval& other) const {
			return get_start() < other.get_start();
		}

		bool operator !=(const interval& other) const {
			return get_start() != other.get_start() || get_end() != other.get_end();
		}

		bool operator ==(const interval& other) const {
			return get_start() == other.get_start() && get_end() == other.get_end();
		}
	};


	/// <summary>
	/// 	The interval_tree class is a data structure used for storing and querying intervals.
	///		It's implemented as a binary tree where each node represents a range of values and contains intervals that overlap with that range.
	///		It includes:
	///		    interval_collection: This is an alias for std::vector<T>, where T represents the type of intervals stored in the tree.
	///			node class: A class representing a node in the binary tree. 
	///				Each node contains a point (d_point) which represents the median value of the intervals it contains, 
	///				left and right child nodes (d_left and d_right), and a collection of intervals (d_intervals). 
	///			remove_overlaps method: This method removes overlapping intervals from the given collection of intervals.
	///			find_overlaps method : This method finds intervals in the tree that overlap with a given interval i.
	/// </summary>
	template<typename T>
	class interval_tree {
	public:
		using interval_collection = std::vector<T>;

	private:
		// class node
		class node {
			enum direction {
				LEFT, RIGHT
			};
			using node_ptr = std::unique_ptr<node>;

			size_t              d_point;
			node_ptr            d_left;
			node_ptr            d_right;
			interval_collection d_intervals;

		public:
			explicit node(const interval_collection& intervals)
				: d_point(0)
				, d_left(nullptr)
				, d_right(nullptr)
				, d_intervals()
			{
				d_point = determine_median(intervals);
				interval_collection to_left, to_right;
				for (const auto& i : intervals) {
					if (i.get_end() < d_point) {
						to_left.push_back(i);
					} else if (i.get_start() > d_point) {
						to_right.push_back(i);
					} else {
						d_intervals.push_back(i);
					}
				}
				if (to_left.size() > 0) {
					d_left.reset(new node(to_left));
				}
				if (to_right.size() > 0) {
					d_right.reset(new node(to_right));
				}
			}

			size_t determine_median(const interval_collection& intervals) const {
				auto start = std::numeric_limits<size_t>::max();
				auto end   = std::numeric_limits<size_t>::max();
				for (const auto& i : intervals) {
					auto cur_start = i.get_start();
					auto cur_end = i.get_end();
					if (start == std::numeric_limits<size_t>::max() || cur_start < start) {
						start = cur_start;
					}
					if (end == std::numeric_limits<size_t>::max() || cur_end > end) {
						end = cur_end;
					}
				}
				return (start + end) / 2;
			}

			interval_collection find_overlaps(const T& i) {
				interval_collection overlaps;
				if (d_point < i.get_start()) {
					add_to_overlaps(i, overlaps, find_overlapping_ranges(d_right, i));
					add_to_overlaps(i, overlaps, check_right_overlaps(i));
				} else if (d_point > i.get_end()) {
					add_to_overlaps(i, overlaps, find_overlapping_ranges(d_left, i));
					add_to_overlaps(i, overlaps, check_left_overlaps(i));
				} else {
					add_to_overlaps(i, overlaps, d_intervals);
					add_to_overlaps(i, overlaps, find_overlapping_ranges(d_left, i));
					add_to_overlaps(i, overlaps, find_overlapping_ranges(d_right, i));
				}
				return interval_collection(overlaps);
			}

		protected:
			void add_to_overlaps(const T& i, interval_collection& overlaps, interval_collection new_overlaps) const {
				for (const auto& cur : new_overlaps) {
					if (cur != i) {
						overlaps.push_back(cur);
					}
				}
			}

			interval_collection check_left_overlaps(const T& i) const {
				return interval_collection(check_overlaps(i, LEFT));
			}

			interval_collection check_right_overlaps(const T& i) const {
				return interval_collection(check_overlaps(i, RIGHT));
			}

			interval_collection check_overlaps(const T& i, direction d) const {
				interval_collection overlaps;
				for (const auto& cur : d_intervals) {
					switch (d) {
					case LEFT:
						if (cur.get_start() <= i.get_end()) {
							overlaps.push_back(cur);
						}
						break;
					case RIGHT:
						if (cur.get_end() >= i.get_start()) {
							overlaps.push_back(cur);
						}
						break;
					}
				}
				return interval_collection(overlaps);
			}

			interval_collection find_overlapping_ranges(node_ptr& node, const T& i) const {
				if (node) {
					return interval_collection(node->find_overlaps(i));
				}
				return interval_collection();
			}
		};
		node d_root;

	public:
		explicit interval_tree(const interval_collection& intervals)
			: d_root(intervals) {}

		interval_collection remove_overlaps(const interval_collection& intervals) {
			interval_collection result(intervals.begin(), intervals.end());
			std::sort(result.begin(), result.end(), [](const T& a, const T& b) -> bool {
				if (b.size() - a.size() == 0) {
					return a.get_start() > b.get_start();
				}
				return a.size() > b.size();
			});
			std::set<T> remove_tmp;
			for (const auto& i : result) {
				if (remove_tmp.find(i) != remove_tmp.end()) {
					continue;
				}
				auto overlaps = find_overlaps(i);
				for (const auto& overlap : overlaps) {
					remove_tmp.insert(overlap);
				}
			}
			for (const auto& i : remove_tmp) {
				result.erase(
					std::find(result.begin(), result.end(), i)
				);
			}
			std::sort(result.begin(), result.end(), [](const T& a, const T& b) -> bool {
				return a.get_start() < b.get_start();
			});
			return interval_collection(result);
		}

		interval_collection find_overlaps(const T& i) {
			return interval_collection(d_root.find_overlaps(i));
		}
	};

	/// <summary>
	///		The emit class is a representation of an emitted token or substring found during text parsing.
	///		The emitted substring will be registered by it's interval [start_idx, end_idx].
	///		The emit list (d_emits) is stored at the end of string (terminal nodes).
	///		Note:
	///			For multiple insertion of the *same* string, the TRIE doesn't change, but the emit list *does*.
	///			This means that if the TRIE had the string: d -> o -> g -> $ [d_emits: ['dog']]
	///			After second insertion of the string 'dog': d -> o -> g -> $ [d_emits: ['dog', 'dog']]
	///			THIS INFORMATION AFFECTS THE TRIE SIZE AND IS CALCULATED AS PERIPHERAL.
	/// </summary>
	template<typename CharType>
	class emit: public interval {
	public:
		typedef std::basic_string<CharType>  string_type;
		typedef std::basic_string<CharType>& string_ref_type;

	private:
		string_type d_keyword;
		unsigned    d_index = 0;

	public:
		emit()
			: interval(-1, -1)
			, d_keyword() {}

		emit(size_t start, size_t end, string_type keyword, unsigned index)
			: interval(start, end)
			, d_keyword(keyword), d_index(index) {}

		string_type get_keyword() const { return string_type(d_keyword); }
		unsigned get_index() const { return d_index; }
		bool is_empty() const { return (get_start() == -1 && get_end() == -1); }
	};

	/// <summary>
	///		The token class represents a token generated during text parsing.
	/// </summary>
	template<typename CharType>
	class token {
	public:
		enum token_type{
			TYPE_FRAGMENT,
			TYPE_MATCH,
		};

		using string_type     = std::basic_string<CharType>;
		using string_ref_type = std::basic_string<CharType>&;
		using emit_type       = emit<CharType>;

	private:
		token_type  d_type;
		string_type d_fragment;
		emit_type   d_emit;

	public:
		explicit token(string_ref_type fragment)
			: d_type(TYPE_FRAGMENT)
			, d_fragment(fragment)
			, d_emit() {}

		token(string_ref_type fragment, const emit_type& e)
			: d_type(TYPE_MATCH)
			, d_fragment(fragment)
			, d_emit(e) {}

		bool is_match() const { return (d_type == TYPE_MATCH); }
		string_type get_fragment() const { return string_type(d_fragment); }
		emit_type get_emit() const { return d_emit; }
	};

	/// <summary>
	/// A class representing a State in the aho-corasick automaton (trie tree).
	/// </summary>
	template<typename CharType>
	class state {
	public:
		typedef state<CharType>*                 ptr;
		typedef std::unique_ptr<state<CharType>> unique_ptr;
		typedef std::basic_string<CharType>      string_type;
		typedef std::basic_string<CharType>&     string_ref_type;
		typedef std::pair<string_type, unsigned> key_index;
		typedef std::set<key_index>              string_collection;
		typedef std::vector<ptr>                 state_collection;
		typedef std::vector<CharType>            transition_collection;

	private:
		size_t                         d_depth;
		ptr                            d_root;
		std::map<CharType, unique_ptr> d_success;		// Effective list of transitions and chars
														// For every state in the automaton:
														//		map: {char_of_next_state(s) : ptr_to_follow_up_state}
														//		Bla => ['B', ptr_B] -> ['l', ptr_l] -> ['a', ptr_a]
														//		map_item.first = the char of the state
														//		map_item.second = the ptr to the state
		ptr                            d_failure;
		string_collection              d_emits;			// Emits, list of full keywords stored in terminal nodes

	public:
		state(): state(0) {}

		explicit state(size_t depth)
			: d_depth(depth)
			, d_root(depth == 0 ? this : nullptr)
			, d_success()
			, d_failure(nullptr)
			, d_emits() {}

		/// <summary>
		/// Returns the size of a node in Bytes.
		///	Usage:
		///		.get_size() - returns node size without emits list or peripherals
		///		.get_size(true) - returns node size with emits list but without peripherals
		///		.get_size(true, true) - returns full node size with emits list and peripherals
		/// </summary>
		/// <param name="include_emits">A Boolean to determine whether or not to calculate a node's emit list in the total size.</param>
		/// <param name="include_peripherals">A Boolean to determine whether or not to calculate a node's peripherals in the total size.</param>
		/// <returns></returns>
		size_t get_size(bool include_emits = false, bool include_peripherals = false) const {
			size_t calculated_size = 0;
			
			size_t map_element = sizeof(CharType) + sizeof(unique_ptr);
			size_t num_of_map_elements = d_success.size();

			calculated_size += map_element * num_of_map_elements;
			
			if (include_peripherals) {
				calculated_size += sizeof(d_depth) + sizeof(d_root) + sizeof(d_failure);
			}

			if (include_emits) {
				for (auto e : d_emits) {
					calculated_size += sizeof(e.second);
					for (CharType c : e.first) {
						calculated_size += sizeof(CharType);
					}
				}
			}
			return calculated_size;
		}

		ptr next_state(CharType character) const {
			return next_state(character, false);
		}

		ptr next_state_ignore_root_state(CharType character) const {
			return next_state(character, true);
		}

		ptr add_state(CharType character) {
			auto next = next_state_ignore_root_state(character);
			if (next == nullptr) {
				next = new state<CharType>(d_depth + 1);
				d_success[character].reset(next);
			}
			return next;
		}

		size_t get_depth() const { return d_depth; }

		void add_emit(string_ref_type keyword, unsigned index) {
			d_emits.insert(std::make_pair(keyword, index));
		}

		void add_emit(const string_collection& emits) {
			for (const auto& e : emits) {
				string_type str(e.first);
				add_emit(str, e.second);
			}
		}

		string_collection get_emits() const { return d_emits; }

		ptr failure() const { return d_failure; }

		void set_failure(ptr fail_state) { d_failure = fail_state; }

		state_collection get_states() const {
			state_collection result;
			for (auto it = d_success.cbegin(); it != d_success.cend(); ++it) {
				result.push_back(it->second.get());
			}
			return state_collection(result);
		}

		transition_collection get_transitions() const {
			transition_collection result;
			for (auto it = d_success.cbegin(); it != d_success.cend(); ++it) {
				result.push_back(it->first);
			}
			return transition_collection(result);
		}

	private:
		ptr next_state(CharType character, bool ignore_root_state) const {
			ptr result = nullptr;
			auto found = d_success.find(character);
			if (found != d_success.end()) {
				result = found->second.get();
			} else if (!ignore_root_state && d_root != nullptr) {
				result = d_root;
			}
			return result;
		}
	};


	/// <summary>
	///		A class that implements the Aho-Corasick automaton using a TRIE tree skeleton.
	///		aho_corasick::trie is defined as basic_trie<char>.
	///		Note: CharType has to be of types char: {char, wchar_t, char32_t, char64_t,...}.
	///			  Use std::basic_string to deal with strings that has NPSCs (Non Printable and Special Characters).
	/// </summary>
	/// <typeparam name="CharType"></typeparam>
	template<typename CharType>
	class basic_trie {
	public:
		using string_type = std::basic_string < CharType >;
		using string_ref_type = std::basic_string<CharType>&;

		typedef state<CharType>         state_type;
		typedef state<CharType>*		state_ptr_type;
		typedef token<CharType>         token_type;
		typedef emit<CharType>          emit_type;
		typedef std::vector<token_type> token_collection;
		typedef std::vector<emit_type>  emit_collection;
		typedef basic_trie<CharType>	this_trie_type;

		class config {
			bool d_allow_overlaps;
			bool d_only_whole_words;
			bool d_case_insensitive;

		public:
			config()
				: d_allow_overlaps(DEFAULT_OVERLAPS)
				, d_only_whole_words(DEFAULT_WHOLE_WORDS)
				, d_case_insensitive(DEFAULT_INSENSITIVE) {}

			bool is_allow_overlaps() const { return d_allow_overlaps; }
			void set_allow_overlaps(bool val) { d_allow_overlaps = val; }

			bool is_only_whole_words() const { return d_only_whole_words; }
			void set_only_whole_words(bool val) { d_only_whole_words = val; }

			bool is_case_insensitive() const { return d_case_insensitive; }
			void set_case_insensitive(bool val) { d_case_insensitive = val; }
		};

	private:
		std::unique_ptr<state_type> d_root;
		config                      d_config;
		std::atomic_bool            d_constructed_failure_states;
		unsigned                    d_num_keywords = 0;
		mutable std::mutex			d_mutex;

	public:
		basic_trie() : basic_trie(config()) {}

		basic_trie(const config& c)
			: d_root(new state_type())
			, d_config(c)
			, d_constructed_failure_states(false) {}

		basic_trie& case_insensitive() {
			d_config.set_case_insensitive(true);
			return (*this);
		}

		basic_trie& remove_overlaps() {
			d_config.set_allow_overlaps(false);
			return (*this);
		}

		basic_trie& only_whole_words() {
			d_config.set_only_whole_words(true);
			return (*this);
		}

		void insert(string_type keyword) {
			if (keyword.empty())
				return;
			state_ptr_type cur_state = d_root.get();
			for (const auto& ch : keyword) {
				cur_state = cur_state->add_state(ch);
			}
			cur_state->add_emit(keyword, d_num_keywords++);
			d_constructed_failure_states.store(false, std::memory_order_relaxed);
		}

		template<class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			for (InputIterator it = first; it != last; ++it) {
				insert(*it);
			}
		}

		token_collection tokenise(string_type text) {
			token_collection tokens;
			auto collected_emits = parse_text(text);
			size_t last_pos = -1;
			for (const auto& e : collected_emits) {
				if (e.get_start() - last_pos > 1) {
					tokens.push_back(create_fragment(e, text, last_pos));
				}
				tokens.push_back(create_match(e, text));
				last_pos = e.get_end();
			}
			if (text.size() - last_pos > 1) {
				tokens.push_back(create_fragment(typename token_type::emit_type(), text, last_pos));
			}
			return token_collection(tokens);
		}
		
		/// <summary>
		/// Scans the aho_corasick TRIE for a text and returns the list of emits (strings) that were found
		/// </summary>
		/// <param name="text">A text to find exact matches on using the aho_corasick automaton</param>
		/// <returns>An std::vector of the emits (strings with relevant intervals) found</returns>
		emit_collection parse_text(string_type text) const {
			check_construct_failure_states();
			size_t pos = 0;
			state_ptr_type cur_state = d_root.get();
			emit_collection collected_emits;
			for (auto c : text) {
				if (d_config.is_case_insensitive()) {
					c = std::tolower(c);
				}
				cur_state = get_state(cur_state, c);
				store_emits(pos, cur_state, collected_emits);
				pos++;
			}
			if (d_config.is_only_whole_words()) {
				remove_partial_matches(text, collected_emits);
			}
			if (!d_config.is_allow_overlaps()) {
				interval_tree<emit_type> tree(typename interval_tree<emit_type>::interval_collection(collected_emits.begin(), collected_emits.end()));
				auto tmp = tree.remove_overlaps(collected_emits);
				collected_emits.swap(tmp);
			}
			return emit_collection(collected_emits);
		}

		/// <summary>
		/// Traverse the Aho Corasick TRIE tree to 
		/// </summary>
		/// <param name="include_emits">A Boolean to determine whether or not to calculate a node's emit list in the total size.</param>
		/// <param name="include_peripherals">A Boolean to determine whether or not to calculate a node's peripherals in the total size.</param>
		/// <param name="print">A Boolean to determine whether or not to print the emits when traversing the tree</param>
		/// <returns></returns>
		size_t traverse_tree(bool include_emits = false, bool include_peripherals = false, bool print = false) const {
			size_t size = 0;
			this->traverse_tree_aux(d_root.get(), &size, include_emits, include_peripherals, print);
			return size;
		}

		size_t getNumKeywords() const {
			return this->d_num_keywords;
		}

	private:
		token_type create_fragment(const typename token_type::emit_type& e, string_ref_type text, size_t last_pos) const {
			auto start = last_pos + 1;
			auto end = (e.is_empty()) ? text.size() : e.get_start();
			auto len = end - start;
			typename token_type::string_type str(text.substr(start, len));
			return token_type(str);
		}

		token_type create_match(const typename token_type::emit_type& e, string_ref_type text) const {
			auto start = e.get_start();
			auto end = e.get_end() + 1;
			auto len = end - start;
			typename token_type::string_type str(text.substr(start, len));
			return token_type(str, e);
		}

		void remove_partial_matches(string_ref_type search_text, emit_collection& collected_emits) const {
			size_t size = search_text.size();
			emit_collection remove_emits;
			for (const auto& e : collected_emits) {
				if ((e.get_start() == 0 || !std::isalpha(search_text.at(e.get_start() - 1))) &&
					(e.get_end() + 1 == size || !std::isalpha(search_text.at(e.get_end() + 1)))
					) {
					continue;
				}
				remove_emits.push_back(e);
			}
			for (auto& e : remove_emits) {
				collected_emits.erase(
					std::find(collected_emits.begin(), collected_emits.end(), e)
					);
			}
		}

		state_ptr_type get_state(state_ptr_type cur_state, CharType c) const {
			state_ptr_type result = cur_state->next_state(c);
			while (result == nullptr) {
				cur_state = cur_state->failure();
				result = cur_state->next_state(c);
			}
			return result;
		}

		void check_construct_failure_states() const {
			bool constructed = d_constructed_failure_states.load(std::memory_order_acquire);
			if (!constructed) {
				std::unique_lock<std::mutex> lock(d_mutex);
				constructed = d_constructed_failure_states.load(std::memory_order_relaxed);
				if(!constructed) {
					const_cast<this_trie_type*>(this)->construct_failure_states();
				}
			}
		}

		void construct_failure_states() {
			std::queue<state_ptr_type> q;
			for (auto& depth_one_state : d_root->get_states()) {
				depth_one_state->set_failure(d_root.get());
				q.push(depth_one_state);
			}
			//d_constructed_failure_states = true;

			while (!q.empty()) {
				auto cur_state = q.front();
				for (const auto& transition : cur_state->get_transitions()) {
					state_ptr_type target_state = cur_state->next_state(transition);
					q.push(target_state);

					state_ptr_type trace_failure_state = cur_state->failure();
					while (trace_failure_state->next_state(transition) == nullptr) {
						trace_failure_state = trace_failure_state->failure();
					}
					state_ptr_type new_failure_state = trace_failure_state->next_state(transition);
					target_state->set_failure(new_failure_state);
					target_state->add_emit(new_failure_state->get_emits());
				}
				q.pop();
			}
			d_constructed_failure_states.store(true, std::memory_order_release);
		}

		void store_emits(size_t pos, state_ptr_type cur_state, emit_collection& collected_emits) const {
			auto emits = cur_state->get_emits();
			if (!emits.empty()) {
				for (const auto& str : emits) {
					auto emit_str = typename emit_type::string_type(str.first);
					collected_emits.push_back(emit_type(pos - emit_str.size() + 1, pos, emit_str, str.second));
				}
			}
		}


		/// <summary>
		/// An Auxiliary function that recursively traverse the TRIE tree in pre-orderly manner.
		/// </summary>
		/// <param name="node">Node to inspect</param>
		/// <param name="size_ptr">A pointer to a paramter that holds the sum value of the tree size in Bytes</param>
		/// <param name="include_emits">A Boolean to determine whether or not to calculate a node's emit list in the total size.</param>
		/// <param name="include_peripherals">A Boolean to determine whether or not to calculate a node's peripherals in the total size.</param>
		/// <param name="print">A Boolean to determine whether or not to print the emits when traversing the tree</param>
		void traverse_tree_aux(const typename basic_trie<CharType>::state_ptr_type& node, size_t* size_ptr,
			bool include_emits = false, bool include_peripherals = false, bool print = false) const {
			
			// Stop condition: NULLPTR (no further sons / transitions to node)
			if (!node) {
				return;
			}
			
			// Pre-Order traversal (NLR): First apply function (sum) on Node, then call children Left to Right.
			*size_ptr += node->get_size(include_emits, include_peripherals);
			
			// Print the current state's or node's details (emits list <=> terminal node)
			if (print) {
				std::cout << "Emits: ";
				for (const auto& emit : node->get_emits()) {
					std::cout << emit.first << " ";
				}
				std::cout << std::endl;
			}

			// Recursively traverse child states
			for (const auto& transition : node->get_transitions()) {
				// Print transitions (chars of next states in TRIE)
				if (print) {
					std::cout << transition << ",  ";
				}
				auto child_state = node->next_state(transition);
				this->traverse_tree_aux(child_state, size_ptr, include_emits, include_peripherals);
			}
		}
	};

	typedef basic_trie<char>     trie;
	typedef basic_trie<wchar_t>  wtrie;


} // namespace aho_corasick

#endif // AHO_CORASICK_HPP
