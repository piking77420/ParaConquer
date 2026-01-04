#pragma once

#include <array>

#include "RhiResource.hpp"

BEGIN_PCCORE

class RhiRenderPass;

class RhiShaderProgram : public RhiObjectT<RhiShaderProgram>
{
public:
    enum class PipelineType
    {
        None,
        Graphic,
        Compute,
        RayTracing,
        MeshShader,

        Count
    };

    enum ShaderStageTypeBits : uint16_t
    {
        Vertex = 1 << 0,
        Hull = 1 << 1,
        Domain = 1 << 2,
        Geometry = 1 << 3,
        Pixel = 1 << 4,
        Compute = 1 << 5,
        Raygen = 1 << 6,
        Intersection = 1 << 7,
        Anyhit = 1 << 8,
        Closesthit = 1 << 9,
        Miss = 1 << 10,
        Callable = 1 << 11,
        Task = 1 << 12,
        Mesh = 1 << 13,

        ShaderStageTypeCount = 14
    };

    using ShaderModule = std::pair<ShaderStageTypeBits, std::vector<char>>;

    static constexpr const std::array<std::string_view, static_cast<size_t>(ShaderStageTypeBits::ShaderStageTypeCount)> ShaderSourceFormat =
    {
        ".vs.hlsl",
        ".hs.hlsl",
        ".ds.hlsl",
        ".gs.hlsl",
        ".ps.hlsl",
        ".cs.hlsl",
        ".rgen.hlsl",
        ".rint.hlsl",
        ".ahit.hlsl",
        ".chit.hlsl",
        ".miss.hlsl",
        ".call.hlsl",
        ".task.hlsl",
        ".mesh.hlsl",
    };

    enum class PolygonMode
    {
        Fill,
        Line,
        Point,
        FillRectangleNV
    };

    enum PrimitiveTopology
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

        PrimitiveTopologyCount
    };

    enum CullModeFlagBits
    {
        CullNone = 0,
        CullFront = 1 << 1,
        CullBack = 1 << 2,
        CullFrontAndBack = 1 << 3,
    };

    using CullModeFlag = uint32_t;

    enum class FrontFace
    {
        CounterClockwise,
        Clockwise
    };

    struct BlendState
    {
        bool enabled = false;
        BlendFactor srcColorBlendFactor = BlendFactor::One;
        BlendFactor dstColorBlendFactor = BlendFactor::Zero;
        BlendOp colorBlendOp = BlendOp::Add;

        BlendFactor srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp alphaBlendOp = BlendOp::Add;
        ColorComponent colorMask = static_cast<ColorComponent>(ColorComponentR | ColorComponentG | ColorComponentB |
            ColorComponentA);
    };

    struct DephStencilInfo
    {
        bool enableDepthTest = false;
        bool enableDepthWrite = false;
        CompareOp depthCompareOp = CompareOp::Less;
    };

    struct LocalSize
    {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

protected:

    struct GraphicPipelineData
    {
        PolygonMode PolygonMode{ PolygonMode::Fill };
        CullModeFlag CullMode{ 0u };
        uint32_t Sample{ 1u };
        FrontFace FrontFace{ FrontFace::CounterClockwise };
        DephStencilInfo DephStencilInfo;
        BlendState BlendState;
        PrimitiveTopology PrimitiveTopology{ PrimitiveTopology::PrimitiveTopologyTriangleList };

        RhiRenderPass* RenderPass{ nullptr };
        uint32_t attachementCount{ 0u };
        uint32_t subPassIndex{ 0u };

        std::vector<VertexInputBindingDescrition> vertexInputBindingDescritions;
        std::vector<VertexAttributeDescription> vertexAttributeDescriptions;
    };

    struct ComputePipelineData
    {
        LocalSize LocalSize{};
    };

public:
    PC_CORE_API RhiShaderProgram(Rhi& _Rhi);

    PC_CORE_API ~RhiShaderProgram() override = default;

    // Setter 

    PC_CORE_API RhiShaderProgram& SetPipelineType(PipelineType _Type);

    PC_CORE_API RhiShaderProgram& SetPolygonMode(PolygonMode _PolygonMode)
    {
        std::get<GraphicPipelineData>(m_PipelineData).PolygonMode = _PolygonMode;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetCullMode(CullModeFlag _CullMode)
    {
        std::get<GraphicPipelineData>(m_PipelineData).CullMode = _CullMode;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetSamples(uint32_t _Sample)
    {
        std::get<GraphicPipelineData>(m_PipelineData).Sample = _Sample;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetFrontFace(FrontFace _FrontFace)
    {
        std::get<GraphicPipelineData>(m_PipelineData).FrontFace = _FrontFace;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetDepthTest(bool _DepthTest)
    {
        std::get<GraphicPipelineData>(m_PipelineData).DephStencilInfo.enableDepthTest = _DepthTest;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetDepthWrite(bool _DepthWrite)
    {
        std::get<GraphicPipelineData>(m_PipelineData).DephStencilInfo.enableDepthWrite = _DepthWrite;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetDepthCompareOp(CompareOp _CompareOp)
    {
        std::get<GraphicPipelineData>(m_PipelineData).DephStencilInfo.depthCompareOp = _CompareOp;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetBlendState(BlendState _BlendInfo)
    {
        std::get<GraphicPipelineData>(m_PipelineData).BlendState = _BlendInfo;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetRenderPass(RhiRenderPass& _RhiRenderPass)
    {
        std::get<GraphicPipelineData>(m_PipelineData).RenderPass = &_RhiRenderPass;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetAttachementCount(uint32_t _AttachementCount)
    {
        std::get<GraphicPipelineData>(m_PipelineData).attachementCount = _AttachementCount;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetSubPassIndex(uint32_t _SubPassIndex)
    {
        std::get<GraphicPipelineData>(m_PipelineData).subPassIndex = _SubPassIndex;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetVertexInputBindingDescritions(const std::vector<VertexInputBindingDescrition>& _VertexInputBindingDescritions)
    {
        std::get<GraphicPipelineData>(m_PipelineData).vertexInputBindingDescritions = _VertexInputBindingDescritions;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetVertexAttributeDescriptions(const std::vector<VertexAttributeDescription>& _VertexAttributeDescriptions)
    {
        std::get<GraphicPipelineData>(m_PipelineData).vertexAttributeDescriptions = _VertexAttributeDescriptions;
        return *this;
    }

    PC_CORE_API RhiShaderProgram& SetShaderModules(const std::vector<ShaderModule>& _ShaderModules);


    // Getter

    PC_CORE_API const LocalSize& GetLocalSize() const
    {
        return std::get<ComputePipelineData>(m_PipelineData).LocalSize;
    }


    PC_CORE_API virtual void HotReload(const std::vector<ShaderModule>& _modules) = 0;

protected:
    PipelineType m_Type{PipelineType::None};

    std::optional<std::vector<ShaderModule>> m_Modules;

    std::variant<std::monostate, GraphicPipelineData, ComputePipelineData> m_PipelineData;
};

REFLECT(RhiShaderProgram, RhiResource);

END_PCCORE

using RhiShader = PC_CORE::RhiShaderProgram;
using RhiShaderStageBits = PC_CORE::RhiShaderProgram::ShaderStageTypeBits;
using RhiShaderStageTypeFlag = uint8_t;