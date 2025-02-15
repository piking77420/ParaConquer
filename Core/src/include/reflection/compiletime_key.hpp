#pragma once
#include "simple_type_name.hpp"

//https://stackoverflow.com/questions/2111667/compile-time-string-hashing
consteval uint32_t cx_hash(const char* input) {
    uint32_t hash = sizeof(uint32_t) == 8 ? 0xcbf29ce484222325 : 0x811c9dc5;
    const uint32_t prime = sizeof(uint32_t) == 8 ? 0x00000100000001b3 : 0x01000193;

    while (*input) {
        hash ^= static_cast<uint32_t>(*input);
        hash *= prime;
        ++input;
    }

    return hash;
}

// This don't take into account the nul char
#define COMPILE_TIME_CRC32_STR(x) (cx_hash(x))


#define COMPILE_TIME_TYPE_KEY(T) COMPILE_TIME_CRC32_STR(skydown::short_type_name<T>.data())

