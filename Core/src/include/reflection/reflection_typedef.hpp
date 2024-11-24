#pragma once
#include "core_header.hpp"
#include <vector>
#include <string>

BEGIN_PCCORE


using CreateFunc = void (*)(void*);
using DeleteFunc = void (*)(void*);
using SerializeFunc = std::string (*)(const void*);
using DerializeFunc = void (*)(void*, const std::string&);

enum class DataNature
{
    UNKNOWN,
    CHAR,
    BOOL,
    INT,
    VEC2I,
    VEC3I,
    UINT,
    FLOAT,
    DOUBLE,
    VEC2,
    VEC3,
    VEC4,
    QUAT,
    RESOURCE,
    STRING,
    
    COUNT
};

enum TypeFlag
{
    NONEFlAG,
    COMPOSITE,  // Composite of trivial type
    ARRAY,      // An array
    VECTOR, // An vector
    POINTER     // A pointer to an object
};

struct ArrayInfo
{
    uint32_t typeKeyOfTheArray;
    size_t sizeOfTheArray;
};

struct TypeInfo
{
    DataNature dataNature;
    uint32_t typeInfoFlags;  // Flags that represent different type properties
    ArrayInfo arrayInfo;
};

enum MemberEnumFlag
{
    NONE,
    NOTSERIALIZE,
    COLOR,
    EULER_ANGLES
};



struct Members
{
    uint32_t typeKey = 0;
    std::string membersName;
    size_t offset = 0;
    uintmax_t enumFlag = 0;
};
    
struct ReflectedType
{
    uint32_t HashKey;
    TypeInfo typeInfo;
    
    std::string name;
    size_t typeSize;
    
    std::vector<Members> members;
    // Dont Support MultiHirietence
    std::vector<uint32_t> inheritenceKey;
    
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
    
    SerializeFunc serializeFunc = nullptr;
    
};



END_PCCORE