#pragma once
#include "core_header.hpp"

BEGIN_PCCORE

enum class DataNature
{
    UNKNOW,
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
    COMPOSITE,
    CONTAINER,
    
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
    DataNature type;
};



struct ReflectionType
{
    const char* name = {};
    size_t size = {};
    size_t offset = {};
    DataNature datatype = DataNature::UNKNOW;
};

END_PCCORE