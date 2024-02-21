#ifndef _CUCKOO_HASH_CONFIG_H
#define _CUCKOO_HASH_CONFIG_H


#define NUMBER_OF_TESTS 100
#define MAX_LOAD_FACTOR 0.75        // in [0,1]
#define TABLE_SIZE 256              // in KB
#define SHUFFLE_SEED 2847354131     // prime!

struct Empty {};					// Use for reducing CuckooItem from 2*sizeof(T) bytes, to sizeof(T) + 1 bytes.


#endif	// _CUCKOO_HASH_CONFIG_H