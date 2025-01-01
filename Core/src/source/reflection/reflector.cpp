#include "reflection/reflector.hpp"

using namespace PC_CORE;

const ReflectedType& Reflector::GetType(uint32_t _hash)
{
    return m_RelfectionMap.at(_hash);
}



