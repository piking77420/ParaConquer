#include <Reflection/Reflector.hpp>

#include <Math/ToolBoxTypeDef.hpp>

using namespace PC_CORE;



std::unordered_map<TypeId, ReflectedType>& Reflector::ReflectionMap()
{
    static auto map = std::unordered_map<TypeId, ReflectedType>();

    return map;
}

std::unordered_map<size_t, TypeId>& Reflector::RttiToTypeId()
{
    static auto map = std::unordered_map<size_t, TypeId>();

    return map;
}

std::unordered_map<TypeId, ReflectMapFunction>& Reflector::MapReflectFunction()
{
    static auto map = std::unordered_map<TypeId, ReflectMapFunction>();

    return map;
}

std::unordered_map<TypeId, ReflectMapFunction>& Reflector::UnordoredMapReflectFunction()
{
    static auto map = std::unordered_map<TypeId, ReflectMapFunction>();

    return map;
}

const ReflectedType& Reflector::GetTypeFromRTTI(size_t typeIdFromRtti)
{
    assert(RttiToTypeId().contains(typeIdFromRtti) && "Forgot to reflect your type ?");

    const TypeId id = RttiToTypeId().at(typeIdFromRtti);

    return ReflectionMap().at(id);
}

const ReflectedType& Reflector::GetType(uint32_t _hash)
{
    return ReflectionMap().at(_hash);
}

bool Reflector::isTrivialType(TypeId _id)
{
    return !(GetType(_id).typeFlags & ReflectedType::TypeFlagBits::Composite);
}

bool Reflector::Exist(TypeId typeId)
{
    return ReflectionMap().contains(typeId);
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
