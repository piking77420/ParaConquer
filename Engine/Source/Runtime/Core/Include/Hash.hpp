#ifndef CORE_HASH_H
#define CORE_HASH_H

namespace PC_CORE
{
    static inline void HashCombine(uint32_t& _Seed, uint32_t _Value)
    {
        _Seed ^= _Value + 0x9e3779b9 + (_Seed << 6) + (_Seed >> 2);
    }
}

#endif // CORE_HASH_H
