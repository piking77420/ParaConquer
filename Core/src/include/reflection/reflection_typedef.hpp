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

 
enum TypeFlagBits
{
    NONE       = 0,        // No flags set
  COMPOSITE  = 1 << 0,  // Composite of trivial type
  PTR        = 1 << 1,  // Pointer type
  CONTINUOUS = 1 << 2   // Continuous memory layout
    
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
struct TypeMetaData
{
    TypeId metaDataType = NullTypeId;
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