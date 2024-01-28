#ifndef _CUSTOM_HASH_H
#define _CUSTOM_HASH_H

#include <cstdint>

#define MURMURHASH3_ROUND_SHIFT_UINT64_T 47
#define MURMURHASH3_FIRST_ROUND_SHIFT_UINT32_T 15
#define MURMURHASH3_SECOND_ROUND_SHIFT_UINT32_T 13
#define MURMURHASH3_FIRST_ROUND_SHIFT_UINT16_T 5
#define MURMURHASH3_SECOND_ROUND_SHIFT_UINT16_T 3

#define MURMURHASH3_MULTIPLIER_UINT64_T 0xc6a4a7935bd1e995
#define MURMURHASH3_MULTIPLIER_UINT32_T 0xcc9e2d51
#define MURMURHASH3_MULTIPLIER_UINT16_T 0xc1ae

#define HASH_SEED_UINT64_T 0x9e3779b97f4a7c15       // TODO: check if prime
// #define HASH_SEED_UINT64_T 0x9e3779b97f4a7c13    // TODO: check if prime
#define HASH_SEED_UINT32_T 0x9e3779b1               // prime!
#define HASH_SEED_UINT16_T 0x1f49                   // prime!

/* 
The original MurmurHash3 algorithm, as proposed by Austin Appleby, 
    involves multiple mixing rounds to achieve a good avalanche effect
    and uniform distribution of hash values. The choice of two rounds 
    for uint32_t and uint16_t and one round for uint64_t in the provided 
    examples is a simplification for illustration purposes.
*/


struct CustomHash {
    // OVERLOADING for UINT64_T:
    std::size_t operator()(const uint64_t key) const {
        constexpr uint64_t mult = MURMURHASH3_MULTIPLIER_UINT64_T;
        constexpr uint64_t seed = HASH_SEED_UINT64_T;
        uint64_t hash = seed ^ (key * mult);
        hash ^= (hash >> MURMURHASH3_ROUND_SHIFT_UINT64_T);
        hash *= mult;
        hash ^= (hash >> MURMURHASH3_ROUND_SHIFT_UINT64_T);

        return static_cast<std::size_t>(hash);
    }

    // OVERLOADING for UINT32_T:
    std::size_t operator()(const uint32_t key) const {
        constexpr uint32_t mult = MURMURHASH3_MULTIPLIER_UINT32_T;
        constexpr uint32_t seed = HASH_SEED_UINT32_T;
        uint32_t hash = seed ^ (key * mult);
        hash ^= (hash >> MURMURHASH3_FIRST_ROUND_SHIFT_UINT32_T);
        hash *= mult;
        hash ^= (hash >> MURMURHASH3_SECOND_ROUND_SHIFT_UINT32_T);
        // third round if needed
        //hash *= mult;
        //hash ^= (hash >> 16);

        return static_cast<std::size_t>(hash);
    }


    // OVERLOADING for UINT16_T:
    std::size_t operator()(const uint16_t key) const {
        constexpr uint16_t mult = MURMURHASH3_MULTIPLIER_UINT16_T;
        constexpr uint16_t seed = HASH_SEED_UINT16_T;
        uint16_t hash = seed ^ (key * mult);
        hash ^= (hash >> MURMURHASH3_FIRST_ROUND_SHIFT_UINT16_T);
        hash *= mult;
        hash ^= (hash >> MURMURHASH3_FIRST_ROUND_SHIFT_UINT16_T);
        // third round if needed
        //hash *= mult;
        //hash ^= (hash >> 8);

        return static_cast<std::size_t>(hash);
    }
};


#endif /* _CUSTOM_HASH_H */