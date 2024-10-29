#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

enum GPU_BUFFER_USAGE : int
{
    BUFFER_USAGE_NONE           = 0,
    BUFFER_USAGE_VERTEX         = 1 << 0,  // 1
    BUFFER_USAGE_INDEX          = 1 << 1,  // 2
    BUFFER_USAGE_UNIFORM        = 1 << 2,  // 4
    BUFFER_USAGE_SHADER_STORAGE = 1 << 3,  // 8
    BUFFER_USAGE_TRANSFERT_SRC      = 1 << 4,  // 16
    BUFFER_USAGE_TRANSFERT_DST      = 1 << 5,  // 32
    BUFFER_USAGE_TEXTURE = 1 << 6,

    COUNT
};


//ENUM_FLAGS(GPU_BUFFER_USAGE);

END_PCCORE