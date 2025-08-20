#include "Resources/ComputeShader.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::ComputeShader::ComputeShader(const std::string& _shaderName,
                                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) : PC_CORE::ShaderProgram(_shaderName, ShaderProgramPipelineType::COMPUTE, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::COMPUTE,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
    };

    const PC_CORE::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderModule = {{ShaderStageTypeFlag::Closesthit, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}

PC_CORE::ComputeShader::ComputeShader(std::string&& _shaderName,
    const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) : PC_CORE::ShaderProgram(std::move(_shaderName), ShaderProgramPipelineType::COMPUTE, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

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
        .shaderModule = {{ShaderStageTypeFlag::Compute, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}
