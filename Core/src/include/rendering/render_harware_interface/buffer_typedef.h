#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

enum class GPU_BUFFER_USAGE
{
    NONE = 0,
    VERTEX,
    INDEX,
    UNIFORM,
    DYNAMIC_UNIFORM,
    SHADER_STORAGE,
    // TODO REMOVE IT 
    TRANSFERT_SRC,
    
    COUNT 
};

END_PCCORE