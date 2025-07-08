#pragma once

#include <array>

#include "descriptor_set.hpp"
#include "rhi_render_pass.hpp"
#include "rhi_typedef.h"
#include "vertex.hpp"

BEGIN_PCCORE
#pragma region Shader

enum class ShaderStageType : size_t
{
    VERTEX,
    TESSCONTROL,
    TESSEVALUATION,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
    RAYGEN,
    INTERSECT,
    ANYHIT,
    CLOSESTHIT,
    MISS,
    CALLABLE,
    TASK,
    MESH,

    COUNT
};

const std::array<std::string, 14> ShaderSourceFormat =
{
    ".vert",
    ".tessc",
    ".tessv",
    ".geom",
    ".frag",
    ".comp",
    ".raygen",
    ".intersect",
    ".anyhit",
    ".closesthit",
    ".miss",
    ".callable",
    ".task"
    ".mesh",
};


struct ShaderSourcePath
{
    std::string shaderSourceCodePath;
    std::string spvCode;
};



enum class PolygonMode
{
    Fill,
    Line,
    Point,
    FillRectangleNV
};

enum struct PrimitiveTopology
{
    PrimitiveTopologyPointList = 0,
    PrimitiveTopologyLineList = 1,
    PrimitiveTopologyLineStrip = 2,
    PrimitiveTopologyTriangleList = 3,
    PrimitiveTopologyTriangleStrip = 4,
    PrimitiveTopologyTriangle_FAN = 5,
    PrimitiveTopologyLineListWithAdjacency = 6,
    PrimitiveTopologyLineStripWithAdjacency = 7,
    PrimitiveTopologyTriangleListWithAdjacency = 8,
    PrimitiveTopologyTriangleStripWithAdjacency = 9,
    PrimitiveTopologyPathList = 10,

    Count
};

enum class CullModeFlagBit
{
    None,
    Front,
    Back,
    FrontAndBack,
    Count
};

enum class FrontFace
{
    CounterClockwise,
    Clockwise
};

struct RasterizerInfo
{
    PolygonMode polygonMode = PolygonMode::Fill;
    CullModeFlagBit cullModeFlag = CullModeFlagBit::Back;
    FrontFace frontFace = FrontFace::CounterClockwise;
    uint32_t multiSampleRasterization = 1;
};

struct ShaderGraphicPointInfo
{
    RasterizerInfo rasterizerInfo;
    CompareOp depthCompareOp = CompareOp::LESS;
    std::vector<VertexInputBindingDescrition> vertexInputBindingDescritions;
    std::vector<VertexAttributeDescription> vertexAttributeDescriptions;
    bool enableDepthTest = false;
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
    std::vector<std::pair<ShaderStageType, std::string>> shaderSources;
};

struct ProgramShaderCreateInfo
{
    ShaderInfo shaderInfo;
    RhiRenderPass* renderPass;
};

#pragma endregion

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

class RhiShaderProgram : public RhiResource
{
public:
    PC_CORE_API virtual void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set) = 0;
        
    PC_CORE_API virtual void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets) = 0;

    
    PC_CORE_API RhiShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo);

    PC_CORE_API RhiShaderProgram() = default;

    PC_CORE_API virtual ~RhiShaderProgram() = default;
    
protected:
    ProgramShaderCreateInfo m_ProgramShaderCreateInfo;

    
};

END_PCCORE