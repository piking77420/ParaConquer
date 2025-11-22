#include "Resources/ComputeShader.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::ComputeShader::ComputeShader(const std::string& _shaderName,
                                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) :
    ShaderProgram(_shaderName, ShaderProgramPipelineType::Compute, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::Compute,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
    };

    const ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderModule = {{ShaderStageType::Compute, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}

PC_CORE::ComputeShader::ComputeShader(std::string&& _shaderName,
                                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo) :
    ShaderProgram(std::move(_shaderName), ShaderProgramPipelineType::Compute, _computeShaderProgramCreateInfo.source)
{
    DYNAMIC_REFLECT_INIT

    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::Compute,
        .shaderInfoData = _computeShaderProgramCreateInfo.shaderComputeInfo,
        .shaderName = Name
    };

    const ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = nullptr,
        .shaderModule = {{ShaderStageType::Compute, _computeShaderProgramCreateInfo.source.lock()->GetCode()}},
        .attachementCount = 0,
        .subPassIndex = 0,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}
