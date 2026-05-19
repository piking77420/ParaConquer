#include "LowRenderer/RhiShaderProgram.hpp"

namespace PC_CORE
{
PC_CORE::RhiShaderProgram::RhiShaderProgram(Rhi& _Rhi)
    : RhiObjectT(_Rhi)
{
}

RhiShaderProgram& RhiShaderProgram::SetPipelineType(PipelineType _Type)
{
    m_Type = _Type;
    switch (m_Type)
    {
    case PC_CORE::RhiShaderProgram::PipelineType::Graphic:
        m_PipelineData = GraphicPipelineData();
        break;
    case PC_CORE::RhiShaderProgram::PipelineType::Compute:
        m_PipelineData = ComputePipelineData();
        break;
    case PC_CORE::RhiShaderProgram::PipelineType::RayTracing:
        break;
    case PC_CORE::RhiShaderProgram::PipelineType::Count:
        break;
    default:
        break;
    }

    return *this;
}

RhiShaderProgram& RhiShaderProgram::SetShaderModules(const std::vector<ShaderModule>& _ShaderModules)
{
    // TODO CHECK PER MODULES IF SUITABLE FOR PIPELINE TYPE
    m_Modules.emplace(_ShaderModules);

    return *this;
}

}


