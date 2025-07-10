#include "resources/graphic_shader.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::GraphicShader::GraphicShader(const std::string& _shaderName,
                                      const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) : ShaderProgram(_shaderName, ShaderProgramPipelineType::POINT_GRAPHICS, _graphicShaderProgramCreateInfo.sourceList)
{
 
    const ShaderInfo shaderInfo =
  {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const PC_CORE::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderSources = SourceListToSourcePath(_graphicShaderProgramCreateInfo.sourceList)
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
}

PC_CORE::GraphicShader::GraphicShader(std::string&& _shaderName,
    const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo) : ShaderProgram(std::move(_shaderName), ShaderProgramPipelineType::POINT_GRAPHICS, _graphicShaderProgramCreateInfo.sourceList)
{
    const ShaderInfo shaderInfo =
  {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = _graphicShaderProgramCreateInfo.shaderGraphicPointInfo,
    };
    // TODO check if source extension are suitable for pipelyne

    const PC_CORE::ProgramShaderCreateInfo programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = _graphicShaderProgramCreateInfo.renderPass,
        .shaderSources = SourceListToSourcePath(_graphicShaderProgramCreateInfo.sourceList)
    };

    m_RhiShaderProgram = Rhi::CreateRhiShaderProgram(programShaderCreateInfo);
    
}

