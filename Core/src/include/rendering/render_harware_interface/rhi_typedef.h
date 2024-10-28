#pragma once

#include <variant>
#include <vector>

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


using GPUObjectHandle = void*;
using CommandBufferHandle = GPUObjectHandle;
using CommandPoolHandle = GPUObjectHandle;
using GPUBufferHandle = GPUObjectHandle;
using DescriptorSetLayoutHandle = GPUObjectHandle;
using DescriptorPoolHandle = GPUObjectHandle;
using DescriptorSetHandle = GPUObjectHandle;

constexpr GPUBufferHandle NULL_HANDLE = nullptr;

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

enum class DESCRIPTOR_TYPE
{
    SAMPLER = 0,
    COMBINED_IMAGE_SAMPLER = 1,
    SAMPLED_IMAGE = 2,
    STORAGE_IMAGE = 3,
    UNIFORM_TEXEL_BUFFER = 4,
    STORAGE_TEXEL_BUFFER = 5,
    UNIFORM_BUFFER = 6,
    STORAGE_BUFFER = 7,
    UNIFORM_BUFFER_DYNAMIC = 8,
    STORAGE_BUFFER_DYNAMIC = 9,
    INPUT_ATTACHMENT = 10,
    INLINE_UNIFORM_BLOCK = 1000138000,
    ACCELERATION_STRUCTURE_KHR = 1000150000,
    ACCELERATION_STRUCTURE_NV = 1000165000,
    SAMPLE_WEIGHT_IMAGE_QCOM = 1000440000,
    BLOCK_MATCH_IMAGE_QCOM = 1000440001,
    MUTABLE_EXT = 1000351000,
    INLINE_UNIFORM_BLOCK_EXT = INLINE_UNIFORM_BLOCK,
    MUTABLE_VALVE = MUTABLE_EXT,
    COUNT
};

struct DescriptorPoolSize
{
    DESCRIPTOR_TYPE type;
    uint32_t count;
};


#pragma region LOG_TYPE

    enum class LogType
    {
        INFO,
        WARNING,
        ERROR
    };



#pragma endregion

#pragma region RHIFORMAT
enum class RHIFormat
{
    UNDIFINED,
    R8_UNORM,
    R8_SNORM,
    R8_USCALED,

    R32G32_SFLOAT,
    R32G32B32_SFLOAT,

    COUNT
};

#pragma endregion

#pragma region Shader

    enum class ShaderStageType
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION,
        COMPUTE,

        COUNT
    };

    const std::array<std::string, 5> ShaderSourceFormat
    {
        ".vert",
        ".frag",
        ".geom",
        ".tess",
        ".comp"
    };


    struct ShaderSourcePath
    {
        std::string shaderSourceCodePath;
        std::vector<char> spvCode;
    };

    enum class ShaderProgramPipelineType
    {
        POINT_GRAPHICS,
        COMPUTE,
        RAYTRACING,

        COUT
    };

    enum class VertexInputRate
    {
        VERTEX = 0,
        INSTANCE = 1,

        COUNT
    };

    struct VertexInputBindingDescrition
    {
        uint32_t binding = 0;
        uint32_t stride = 0;
        VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
    };

    struct VertexAttributeDescription
    {
        uint32_t binding = 0;
        uint32_t location = 0;
        RHIFormat format = RHIFormat::UNDIFINED;
        uint32_t offset = 0;
    };


    struct ShaderGraphicPointInfo
    {
        std::vector<VertexInputBindingDescrition> vertexInputBindingDescritions;
        std::vector<VertexAttributeDescription> vertexAttributeDescriptions;
    };

    struct ShaderRayTracingInfo
    {
    };

    struct ShaderComputeInfo
    {
    };

    using ShaderInfoData = std::variant<ShaderGraphicPointInfo, ShaderRayTracingInfo, ShaderComputeInfo>;

    struct ShaderInfo
    {
        ShaderProgramPipelineType shaderProgramPipelineType;
        ShaderInfoData shaderInfoData;
    };

    struct ProgramShaderCreateInfo
    {
        std::string prograShaderName;
        ShaderInfo shaderInfo;
        // there shoulbe a renderpass handle
    };


    static inline ShaderStageType ShaderFormatToShaderType(const char* _formatWithPoint)
    {
        for (int i = 0; i < static_cast<int>(ShaderStageType::COUNT); i++)
        {
            if (_stricmp(_formatWithPoint, ShaderSourceFormat[i].c_str()) == 0)
            {
                return static_cast<ShaderStageType>(i);
            }
        }

        // Return a default value or handle the error when no match is found
        return ShaderStageType::COUNT;
    }

#pragma endregion Shader


#pragma region DescriptorInfo

struct DescriptorBufferInfo
    {
        GPUBufferHandle buffer;
        uint32_t offset;
        uint32_t range;
    };

struct DescriptorImageInfo
{
    
};

struct DescriptorTexelBufferViewInfo
{
    
};

#pragma endregion DescriptorInfo

    struct DescriptorWriteSet
    {
        DescriptorSetHandle dstDescriptorSetHandle = nullptr;
        uint32_t dstBinding = 0;
        uint32_t dstArrayElement = 0;
        DESCRIPTOR_TYPE descriptorType;
        uint32_t descriptorCount = 1;
        
        DescriptorBufferInfo* descriptorBufferInfo = nullptr;
        DescriptorImageInfo* descriptorImageInfo = nullptr; 
        DescriptorTexelBufferViewInfo* descriptorTexelBufferViewInfo = nullptr;
    };

END_PCCORE


#define GLOBAL_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1
