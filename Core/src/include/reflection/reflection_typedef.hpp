#pragma once
#include "core_header.hpp"
#include <vector>
#include <string>

BEGIN_PCCORE

using TypeId = uint32_t; 
static constexpr TypeId NullTypeId = 0;
using TypeFlag = uintmax_t;

using CreateFunc = void (*)(void*);
using DeleteFunc = void (*)(void*);
using SerializeFunc = std::string (*)(const void*);
using DerializeFunc = void (*)(void*, const std::string&);
using OnEditFunc = void (*)(void*);
 
enum TypeFlagBits
{
  NONE       = 0,        // No flags set
  COMPOSITE  = 1 << 0,  // Composite of trivial type
};




enum MemberEnumFlag
{
    NONE_MEMBER_ENUM_FLAG,
    NOTSERIALIZE,
    COLOR,
    HIDE_INSPECTOR,
};


struct Members
{
    TypeId typeKey = NullTypeId;
    std::string membersName;
    size_t offset = 0;
    uintmax_t memberFlag = 0;
};



enum struct TypeNatureMetaDataEnum
{
    None,
    Ptr,
    String,
    Array,
    Vector,
    Map,
};

struct Ptr
{
    TypeId type;
};

struct Array
{
    TypeId type;
    size_t size;
};

struct RelfectedString
{
    TypeId type;
};


struct Vector
{
    TypeId type;
};

struct Map
{
    TypeId key;
    TypeId value;
    size_t nbr;
};

struct Set
{
    TypeId type;
    size_t nbr;
};

struct TypeNatureMetaData
{
    TypeNatureMetaDataEnum metaDataTypeEnum;
    union TypeNatureMetaUnion
    {
        Ptr ptr;
        RelfectedString relfectedString;
        Array array;
        Vector vector;
        Map map;
    }metaDataType;
};


struct TypeMetaData
{
    TypeNatureMetaData typeNatureMetaData;
    std::vector<Members> members;

    // Dont Support MultiHirietence
    TypeId baseClass = NullTypeId;
    
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
};

struct ReflectedType
{
    TypeId typeId;
    uintmax_t typeFlags;
    
    std::string name;
    size_t size;
    
    TypeMetaData metaData;

    bool operator==(const ReflectedType& other) const
    {
        return typeId == other.typeId;
    }
};



END_PCCORE