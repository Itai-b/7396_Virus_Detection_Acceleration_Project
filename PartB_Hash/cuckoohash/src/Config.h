#ifndef _CUCKOO_HASH_CONFIG_H
#define _CUCKOO_HASH_CONFIG_H


#define NUMBER_OF_TESTS 100
#define MAX_LOAD_FACTOR 0.75        // in [0,1]
#define TABLE_SIZE 256              // in KB
#define SHUFFLE_SEED 2847354131     // prime!

// Additional data info (for IBLT / raw linked list calculations):
const std::size_t SID_ENTRY_IN_LINKED_LIST = 64; // 32bits for the SID (ranges from 0-999999 => use uint32_t), 32bits for pointer (in x32 architecture)
const std::size_t IBLT_CELL_SIZE = 40;		     // Theoretically using *ONLY VALUE (32bits for the SID bits xor results) and 8 bit bloom filter, gives us 40 bits for each entry

struct Empty {};					// Use for reducing CuckooItem from 2*sizeof(T) bytes, to sizeof(T) + 1 bytes.

/// <summary>
/// This function returns the number of cells in the Invertible Bloom Lookup Table (IBLT) that every entry in the hash table points at.
/// Base on the thorough calculations made in Part D, we logged success_rate in restoring the rules lists using the IBLT, for ranging # cells.
/// This varies for different substrings with different L,G - since it affects the number of rules assigned to each substring.
/// Above 99.9% success rate counts as 100%.
/// </summary>
/// <param name="L">Length of the substrings in the data structure</param>
/// <param name="G">Gap of the substrings in the data structure</param>
/// <param name="success_rate">Wanted success rate [0,1] in restoring the rules list for substrings in the data structure</param>
/// <returns>Number of cells in the IBLT needed to allocate for the data structure to work under the given params</returns>
std::size_t ibltNumOfCells(std::size_t L, std::size_t G, double success_rate) {
	int rounded_success_rate = int(success_rate * 100);
	
	// L = 8, G = 1 case
	if (L == 8 && G == 1) {
		switch (rounded_success_rate) {
			case 100:
				return 128;
			case 99:
				return 32;
			case 95:
				return 8;
			case 85:
				return 4;
			default:
				return 0;
		}
	}

	// L = 8, G = 2 case
	if (L == 8 && G == 2) {
		switch (rounded_success_rate) {
			case 100:
				return 128;
			case 99:
				return 32;
			case 95:
				return 8;
			case 85:
				return 4;
			default:
				return 0;
		}
	}

	// L = 4, G = 1 case
	if (L == 4 && G == 1) {
		switch (rounded_success_rate) {
		case 100:
			return 256;
		case 99:
			return 64;
		case 95:
			return 16;
		case 85:
			return 8;
		default:
			return 0;
		}
	}

	// L = 4, G = 2 case
	if (L == 4 && G == 2) {
		switch (rounded_success_rate) {
		case 100:
			return 256;
		case 99:
			return 64;
		case 95:
			return 16;
		case 85:
			return 8;
		default:
			return 0;
		}
	}

	return 0;
}

#endif	// _CUCKOO_HASH_CONFIG_H