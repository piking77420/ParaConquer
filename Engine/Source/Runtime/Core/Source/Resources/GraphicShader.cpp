#include "Resources/GraphicShader.hpp"

#include "LowRenderer/Rhi.hpp"

/*
PC_CORE::GraphicShader::GraphicShader(Rhi& rhi, const std::string& _shaderName,
                                      const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) :
    ShaderProgram(_shaderName, RhiShaderProgram::PipelineType::Graphic, _graphicShaderProgramCreateInfo.sourceList)
{
    const RhiShaderProgram::ShaderInfo shaderInfo =
    {
        .type = RhiShaderProgram::PipelineType::Graphic,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const RhiShaderProgram::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderModule = SourceListToShaderModules(_graphicShaderProgramCreateInfo.sourceList),
        .attachementCount = _graphicShaderProgramCreateInfo.colorAttachementCount,
        .subPassIndex = _graphicShaderProgramCreateInfo.subPassIndex,
    };

    m_RhiShaderProgram.reset(rhi.CreateRhiShaderProgram(Name, programShaderCreateInfo));
}

PC_CORE::GraphicShader::GraphicShader(Rhi& rhi, std::string&& _shaderName,
                                      const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) :
    ShaderProgram(std::move(_shaderName), RhiShaderProgram::PipelineType::Graphic,
                  _graphicShaderProgramCreateInfo.sourceList)
{
    const RhiShaderProgram::ShaderInfo shaderInfo =
    {
        .type = RhiShaderProgram::PipelineType::Graphic,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const RhiShaderProgram::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderModule = SourceListToShaderModules(_graphicShaderProgramCreateInfo.sourceList),
        .attachementCount = _graphicShaderProgramCreateInfo.colorAttachementCount,
        .subPassIndex = _graphicShaderProgramCreateInfo.subPassIndex,

    };

    m_RhiShaderProgram.reset(rhi.CreateRhiShaderProgram(Name, programShaderCreateInfo));
}
*/