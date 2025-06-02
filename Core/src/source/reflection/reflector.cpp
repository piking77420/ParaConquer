#include "reflection/reflector.hpp"

#include <filesystem>

using namespace PC_CORE;

const ReflectedType& Reflector::GetTypeFromRTTI(size_t typeIdFromRtti)
{
    const TypeId id = m_RttiToTypeId.at(typeIdFromRtti);

    return m_RelfectionMap.at(id);
}


const ReflectedType& Reflector::GetType(uint32_t _hash)
{
    return m_RelfectionMap.at(_hash);
}

bool Reflector::isTrivialType(TypeId _id)
{
    return !(GetType(_id).typeFlags & TypeFlagBits::COMPOSITE);
}

bool Reflector::ContaintTypeFromTypeID(TypeId typeId)
{
    return m_RelfectionMap.contains(typeId);
}

bool Reflector::GetPtrToTypeField(TypeId _id, void* _object, const std::string& _fieldName, void** _outPtrToField)
{
    const auto& t = GetType(_id);

    const auto* m = t.GetMemberByName(_fieldName);
    if (m == nullptr)
    {
        PC_LOGERROR("There is no members name as {} in {} type", _fieldName, t.name);
        return false;
    }

    *_outPtrToField = static_cast<uint8_t*>(_object) + m->offset;
    return true;
}

bool Reflector::GetPtrToTypeField(TypeId _id, const void* _object, const std::string& _fieldName,
    const void** _outPtrToField)
{
    const auto& t = GetType(_id);

    const auto* m = t.GetMemberByName(_fieldName);
    if (m == nullptr)
    {
        PC_LOGERROR("There is no members name as {} in {} type", _fieldName, t.name);
        return false;
    }

    *_outPtrToField = static_cast<const uint8_t*>(_object) + m->offset;
    return true;
}




// REFLECT TRIVIAL TYPE //
REFLECT(bool);
REFLECT(char);
REFLECT(wchar_t);
REFLECT(int);
REFLECT(float);
REFLECT(double);
REFLECT(uint8_t);
REFLECT(uint16_t);
REFLECT(uint32_t);
REFLECT(uint64_t);


using namespace std;
REFLECT(string)
REFLECT(wstring)




