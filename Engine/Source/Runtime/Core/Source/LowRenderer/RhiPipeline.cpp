#include <LowRenderer/RhiPipeline.hpp>
#include <LowRenderer/RhiRenderPass.hpp>

namespace PC_CORE
{

PC_CORE::RhiPipeline::RhiPipeline(Rhi& _Rhi)
    : RhiObjectT(_Rhi)
{

}

RhiPipeline& RhiPipeline::SetPipelineType(PipelineType _Type)
{
    m_Type = _Type;
    switch (m_Type)
    {
    case PC_CORE::RhiPipeline::PipelineType::Graphic:
        m_PipelineData = GraphicPipelineData();
        break;
    case PC_CORE::RhiPipeline::PipelineType::Compute:
        m_PipelineData = ComputePipelineData();
        break;
    case PC_CORE::RhiPipeline::PipelineType::RayTracing:
        break;
    case PC_CORE::RhiPipeline::PipelineType::Count:
        break;
    default:
        break;
    }

    return *this;
}

RhiPipeline& RhiPipeline::SetShaderModules(const std::vector<ShaderModule>& _ShaderModules)
{
    // TODO CHECK PER MODULES IF SUITABLE FOR PIPELINE TYPE
    m_Modules.emplace(_ShaderModules);

    return *this;
}

size_t RhiPipeline::Hash() const
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    uint32_t Seed = 0;

    HashCombine(Seed, static_cast<uint32_t>(m_Type));

    switch (m_Type)
    {
    case PC_CORE::RhiPipeline::PipelineType::Graphic:
    {
        if (std::holds_alternative<GraphicPipelineData>(m_PipelineData))
        {
            const auto& Data = std::get<GraphicPipelineData>(m_PipelineData);
            HashCombine(Seed, static_cast<uint32_t>(Data.PolygonMode));
            HashCombine(Seed, static_cast<uint32_t>(Data.CullMode));
            HashCombine(Seed, static_cast<uint32_t>(Data.Sample));

            // Depht
            const bool HasDepthState = Data.DephStencilInfo.has_value();
            HashCombine(Seed, static_cast<uint32_t>(HasDepthState));
            if (HasDepthState)
                HashCombine(Seed, static_cast<uint32_t>((*Data.DephStencilInfo).Hash()));

            // Blend
            const bool HasBlendtate = Data.BlendState.has_value();
            HashCombine(Seed, static_cast<uint32_t>(HasBlendtate));
            if (HasBlendtate)
            {
                HashCombine(Seed, static_cast<uint32_t>((*Data.BlendState).Hash()));
            }

            HashCombine(Seed, static_cast<uint32_t>(Data.PrimitiveTopology));
            assert(Data.RenderPass);
            HashCombine(Seed, Data.RenderPass->Hash());
            HashCombine(Seed, Data.subPassIndex);
            HashCombine(Seed, static_cast<uint32_t>(Data.vertexInputBindingDescritions.size()));
            for (const auto& VertexInputBindingDescrition : Data.vertexInputBindingDescritions)
            {
                HashCombine(Seed, static_cast<uint32_t>(VertexInputBindingDescrition.Binding));
                HashCombine(Seed, static_cast<uint32_t>(VertexInputBindingDescrition.Stride));
                HashCombine(Seed, static_cast<uint32_t>(VertexInputBindingDescrition.VertexInputRate));
            }

            HashCombine(Seed, static_cast<uint32_t>(Data.vertexAttributeDescriptions.size()));
            for (const auto& VertexAttributeDescriptions : Data.vertexAttributeDescriptions)
            {
                HashCombine(Seed, static_cast<uint32_t>(VertexAttributeDescriptions.Binding));
                HashCombine(Seed, static_cast<uint32_t>(VertexAttributeDescriptions.Location));
                HashCombine(Seed, static_cast<uint32_t>(VertexAttributeDescriptions.Format));
                HashCombine(Seed, static_cast<uint32_t>(VertexAttributeDescriptions.Offset));
            }

        }
    }
        break;
    case PC_CORE::RhiPipeline::PipelineType::Compute:
        break;
    case PC_CORE::RhiPipeline::PipelineType::RayTracing:
        assert(false);
        break;
    default:
        break;
    }

    return Seed;
}

}


