#include <LowRenderer/RhiPipeline.hpp>

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
    uint32_t seed = 0;
    auto combine = [&seed](uint32_t h) {
        seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };
    combine(static_cast<uint32_t>(m_Type));

    switch (m_Type)
    {
    case PC_CORE::RhiPipeline::PipelineType::Graphic:
    {
        if (std::holds_alternative<GraphicPipelineData>(m_PipelineData))
        {
            const auto& Data = std::get<GraphicPipelineData>(m_PipelineData);
            combine(static_cast<uint32_t>(Data.PolygonMode));
            combine(static_cast<uint32_t>(Data.CullMode));
            combine(static_cast<uint32_t>(Data.Sample));
            //combine(static_cast<uint32_t>(Data.DephStencilInfo));
           // combine(static_cast<uint32_t>(Data.BlendState));
            combine(static_cast<uint32_t>(Data.PrimitiveTopology));
            //combine(static_cast<uint32_t>(Data.RenderPass)); // TODO

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

    return seed;
}

}


