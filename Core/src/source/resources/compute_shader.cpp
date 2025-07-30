#include "resources/compute_shader.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::ComputeShader::ComputeShader(const std::string& _shaderName,
                                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) : PC_CORE::ShaderProgram(_shaderName, ShaderProgramPipelineType::COMPUTE, _computeShaderProgramCreateInfo.source)
{
    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::COMPUTE,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
    };

    const PC_CORE::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderSources = {{ShaderStageTypeFlag::Closesthit, _computeShaderProgramCreateInfo.source.lock()->GetPath()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}

PC_CORE::ComputeShader::ComputeShader(std::string&& _shaderName,
    const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) : PC_CORE::ShaderProgram(std::move(_shaderName), ShaderProgramPipelineType::COMPUTE, _computeShaderProgramCreateInfo.source)
{
    const ShaderInfo shaderInfo =
 {
        .shaderProgramPipelineType = ShaderProgramPipelineType::COMPUTE,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
        .shaderName = name
    };

    const PC_CORE::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderSources = {{ShaderStageTypeFlag::Compute, _computeShaderProgramCreateInfo.source.lock()->GetPath()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}
