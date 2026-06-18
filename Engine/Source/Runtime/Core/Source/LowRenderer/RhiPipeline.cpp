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

}


