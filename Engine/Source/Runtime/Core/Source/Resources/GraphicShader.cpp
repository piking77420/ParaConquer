#include "Resources/GraphicShader.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::GraphicShader::GraphicShader(const std::string& _shaderName,
                                      const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) :
    ShaderProgram(_shaderName, ShaderProgramPipelineType::Graphic, _graphicShaderProgramCreateInfo.sourceList)
{
    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::Graphic,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderModule = SourceListToShaderModules(_graphicShaderProgramCreateInfo.sourceList),
        .attachementCount = _graphicShaderProgramCreateInfo.colorAttachementCount,
        .subPassIndex = _graphicShaderProgramCreateInfo.subPassIndex,
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}

PC_CORE::GraphicShader::GraphicShader(std::string&& _shaderName,
                                      const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) :
    ShaderProgram(std::move(_shaderName), ShaderProgramPipelineType::Graphic,
                  _graphicShaderProgramCreateInfo.sourceList)
{
    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::Graphic,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderModule = SourceListToShaderModules(_graphicShaderProgramCreateInfo.sourceList),
        .attachementCount = _graphicShaderProgramCreateInfo.colorAttachementCount,
        .subPassIndex = _graphicShaderProgramCreateInfo.subPassIndex,

    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}
