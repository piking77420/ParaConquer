#pragma once

#include "shader_typedef.h"
#include "math/toolbox_typedef.hpp"
#include "rendering/render_harware_interface/buffer_typedef.h"


constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
#define ALIGNAS_16 alignas(16)



BEGIN_PCCORE


struct ALIGNAS_16 SceneBufferGPU
{
    Tbx::Matrix4x4f view;
    Tbx::Matrix4x4f proj;
    float time;
    float deltatime;
};

struct ALIGNAS_16 DrawObjectBufferGPU
{
    Tbx::Matrix4x4f model;
};


enum class QueuType
{
    NONE,
    GRAPHICS,
    COMPUTE,
    TRANSFERT,
    SPARSE_BINDING,
    VIDEO_DECODE,
    VIDEO_ENCODE,
    OPTICAL_FLOW,

    COUT
};
ENUM_FLAGS(QueuType)

   
using ObjectHandle = void*;
using CommandBufferHandle = ObjectHandle;
using CommandPoolHandle = ObjectHandle;
using GPUBufferHandle = ObjectHandle;
constexpr void* INVALID_HANDLE = nullptr;

enum CommandPoolBufferFlag
{
    COMMAND_POOL_BUFFER_NONE = 0,
    COMMAND_POOL_BUFFER_RESET = 1 << 0,
    COMMAND_POOL_BUFFER_TRANSIENT = 1 << 1,
    COMMAND_POOL_BUFFER_PROTECTED = 1 << 2,

    COMMAND_POOL_BUFFER_COUNT = std::numeric_limits<int>::max()
};


struct ViewPort
{
    Tbx::Vector2f position;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

struct ScissorRect
{
    Tbx::Vector2f offset;
    Tbx::Vector2ui extend;
};


struct CommandPoolCreateInfo
{
    QueuType queueType = QueuType::NONE;
    CommandPoolBufferFlag commandPoolBufferFlag;
};

enum class CommandBufferlevel
{
    PRIMARY,
    SECONDARY,

    COUT
};

#pragma region LOG_TYPE

    enum class LogType
    {
        INFO,
        WARNING,
        ERROR
    };



#pragma endregion



END_PCCORE

#define CastToObject(type, handle) type(reinterpret_cast<typename type::CType>(handle))

#define GLOBAL_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1
