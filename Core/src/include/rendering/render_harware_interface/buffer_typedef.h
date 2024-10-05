#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

enum class GPU_BUFFER_USAGE
{
    VERTEX,
    INDEX,
    UNIFORM,
    SHADER_STORAGE,
    TRANSFERT,
    
};

enum class BUFFER_FLAG
{
    
};


ENUM_FLAGS(GPU_BUFFER_USAGE);

END_PCCORE