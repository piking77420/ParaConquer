#pragma once

#include <Array>

#include "DescriptorSet.hpp"
#include "RhiRenderPass.hpp"
#include "RhiTypedef.h"
#include "Vertex.hpp"

BEGIN_PCCORE
#pragma region Shader



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

struct BlendInfo
{
    bool enabled = false;
    BlendFactor srcColorBlendFactor = BlendFactor::One;
    BlendFactor dstColorBlendFactor = BlendFactor::Zero;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
    ColorComponent colorMask = static_cast<ColorComponent>(ColorComponent_R | ColorComponent_G | ColorComponent_B | ColorComponent_A);
};

struct DephStencilInfo
{
    CompareOp depthCompareOp = CompareOp::LESS;
    bool enableDepthTest = false;
};

struct LocalSize
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct ShaderGraphicPointInfo
{
    RasterizerInfo rasterizerInfo;
    DephStencilInfo dephInfo;
    BlendInfo blendInfo;
    
    std::vector<VertexInputBindingDescrition> vertexInputBindingDescritions;
    std::vector<VertexAttributeDescription> vertexAttributeDescriptions;
};

struct ShaderRayTracingInfo
{
};

struct ShaderComputeInfo
{
};

using ShaderInfoData = std::variant<ShaderGraphicPointInfo, ShaderComputeInfo, ShaderRayTracingInfo>;


struct ShaderInfo
{
    ShaderProgramPipelineType shaderProgramPipelineType;
    ShaderInfoData shaderInfoData;
    std::string shaderName;
};

using ShaderModule = std::pair<ShaderStageType, std::vector<char>>;

struct ProgramShaderCreateInfo
{
    ShaderInfo shaderInfo;
    RhiRenderPass* renderPass; // is optional TODO check raytracing pipeline use it 
    std::vector<ShaderModule> shaderModule;
    uint32_t attachementCount;
    uint32_t subPassIndex;
};



#pragma endregion Shader

class RhiShaderProgram : public RhiResource
{
public:

    const LocalSize& GetLocalSize() const
    {
        return m_LocalSize;
    }

    PC_CORE_API virtual void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set) = 0;
        
    PC_CORE_API virtual void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets) = 0;

    PC_CORE_API virtual void HotReload(const std::vector<PC_CORE::ShaderModule>& _modules) = 0;

    PC_CORE_API RhiShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo);

    PC_CORE_API RhiShaderProgram() = default;

    PC_CORE_API virtual ~RhiShaderProgram() = default;
    
protected:
    ProgramShaderCreateInfo m_ProgramShaderCreateInfo;

    LocalSize m_LocalSize;
};

REFLECT(RhiShaderProgram, RhiResource);

END_PCCORE