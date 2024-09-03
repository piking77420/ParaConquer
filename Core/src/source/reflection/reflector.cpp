#include "reflection/reflector.hpp"

using namespace PC_CORE;

const ReflectedType& Reflector::GetType(uint32_t _hash)
{
    return m_RelfectionMap.at(_hash);
}

uint32_t Reflector::KR_v2_hash(const char* s)
{
    // Source: https://stackoverflow.com/a/45641002/5407270
    // a.k.a. Java String hashCode()
    uint32_t hashval = 0;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31*hashval;
    return hashval;
}

std::string Reflector::GetCorrectNameFromTypeId(const std::string& _name)
{
    const size_t firstIndex = _name.find_first_of("::",0);
    std::string out;

    if (firstIndex)
    {
        for (size_t i = firstIndex + 2; i < _name.size(); i++)
            out.push_back(_name[i]); 
    }
    else
    {
        const size_t secondIndex = _name.find_first_of(" ",0);
        
        for (size_t i = secondIndex; i < _name.size(); i++)
            out.push_back(_name[i]); 
    }
    
    return out; 
}
