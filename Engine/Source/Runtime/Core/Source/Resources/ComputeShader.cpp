#include "Resources/ComputeShader.hpp"

#include "LowRenderer/Rhi.hpp"

/*
PC_CORE::ComputeShader::ComputeShader(Rhi& rhi, const std::string& _shaderName,
                                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) :
    ShaderProgram(_shaderName, RhiPipeline::PipelineType::Compute, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

    const RhiPipeline::ShaderInfo shaderInfo =
    {
        .type = RhiPipeline::PipelineType::Compute,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
    };

    const RhiPipeline::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderModule = {{RhiPipeline::ShaderStageTypeBits::Compute, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram.reset(rhi.CreateRhiShaderProgram(Name, programShaderCreateInfo));
}

PC_CORE::ComputeShader::ComputeShader(Rhi& rhi, std::string&& _shaderName,
    const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) :
    ShaderProgram(std::move(_shaderName), RhiPipeline::PipelineType::Compute, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

        const RhiPipeline::ShaderInfo shaderInfo =
    {
        .type = RhiPipeline::PipelineType::Compute,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
    };

    const RhiPipeline::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderModule = {{RhiPipeline::ShaderStageTypeBits::Compute, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram.reset(rhi.CreateRhiShaderProgram(Name, programShaderCreateInfo));
}
*/