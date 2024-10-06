#pragma once

#include "shader_typedef.h"
#include "rendering/render_harware_interface/buffer_typedef.h"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;


BEGIN_PCCORE

enum class CommandPoolUsage
{
    NONE,
    GRAPHICS,
    COMPUTE,
    SPARSE_BINDING,
    VIDEO_DECODE,
    OPTICAL_FLOW
    
};
ENUM_FLAGS(CommandPoolUsage)

    
struct CommandPoolCreateInfo
{
    CommandPoolUsage commandPoolUsage;
};


using ObjectHandle = void*;
using CommandBufferHandle = ObjectHandle;
using CommandPoolHandle = ObjectHandle;

enum class CommandBufferFlags
{
    NONE,
    RESET,

    COUNT
};

ENUM_FLAGS(CommandBufferFlags)


END_PCCORE

#define CastToObject(type, handle) type(reinterpret_cast<typename type::CType>(handle))
