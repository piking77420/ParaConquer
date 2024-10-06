#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

enum class GPU_BUFFER_USAGE
{
    NONE,
    VERTEX,
    INDEX,
    UNIFORM,
    SHADER_STORAGE,
    TRANSFERT,

    COUNT
};


ENUM_FLAGS(GPU_BUFFER_USAGE);

END_PCCORE