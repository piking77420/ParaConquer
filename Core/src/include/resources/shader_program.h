#pragma once

#include <array>
#include <vector>
#include <string>

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "low_renderer/rhi_typedef.h"
#include "low_renderer/rhi_render_pass.hpp"
#include "low_renderer/vertex.hpp"

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

enum class ShaderProgramPipelineType
{
    POINT_GRAPHICS,
    COMPUTE,
    RAYTRACING,

    COUT
};


enum class PolygonMode
{
    Fill,
    Line,
    Point,
    FillRectangleNV
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
};

struct ShaderGraphicPointInfo
{
    RasterizerInfo rasterizerInfo;
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
    std::shared_ptr<RhiRenderPass> renderPass;
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

    class ShaderProgram
    {
    public:
        PC_CORE_API virtual void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set) = 0;
        
        PC_CORE_API virtual void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets) = 0;
    
        PC_CORE_API ShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo);

        PC_CORE_API ShaderProgram() = default;

        PC_CORE_API virtual ~ShaderProgram();

    protected:
        ProgramShaderCreateInfo m_ProgramShaderCreateInfo;
    };

END_PCCORE
