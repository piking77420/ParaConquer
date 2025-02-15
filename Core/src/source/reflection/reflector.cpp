#include "reflection/reflector.hpp"

using namespace PC_CORE;

const ReflectedType& Reflector::GetType(uint32_t _hash)
{
    return m_RelfectionMap.at(_hash);
}

bool Reflector::ContaintTypeFromTypeID(TypeId typeId)
{
    return m_RelfectionMap.contains(typeId);
}

// REFLECT TRIVIAL TYPE //
REFLECT(bool);
REFLECT(char);
REFLECT(wchar_t);
REFLECT(int);
REFLECT(float);
REFLECT(double);
REFLECT(uint16_t);
REFLECT(uint32_t);
REFLECT(uint64_t);


using namespace std;
REFLECT(string)
REFLECT(wstring)



