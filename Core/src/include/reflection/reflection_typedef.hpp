#pragma once
#include "core_header.hpp"

BEGIN_PCCORE

enum class DataType
{
    UNKNOW,
    INT,
    VEC2I,
    VEC3I,
    UINT,
    FLOAT,
    DOUBLE,
    VEC2,
    VEC3,
    VEC4,
    COMPOSITE,
    Vector,
    
    COUT
};

enum class ReflectionFlag
{
    COLOR,
    SLIDERANGLES
};

struct MemberDescriptor
{
    size_t nbr;
    DataType type;
};



struct ReflectionType
{
    const char* name = {};
    size_t size = {};
    size_t offset = {};
    DataType datatype = DataType::UNKNOW;
};

END_PCCORE