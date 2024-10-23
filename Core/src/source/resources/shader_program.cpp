#include "resources/shader_program.h"

#include "math/matrix2x2.hpp"
#include "rendering/render_harware_interface/RHI.hpp"

using namespace PC_CORE;

ShaderProgram::ShaderProgram(const ProgramShaderCreateInfo& _createInfo, const std::vector<ShaderSource*>& _shaderSources) :
    Resource()
    , m_ShaderInfo(_createInfo.shaderInfo), m_ShaderSources(_shaderSources)
{
    name = _createInfo.prograShaderName;
    CreateShader();

}



ShaderProgram::~ShaderProgram()
{
    DestroyShader();
}

void ShaderProgram::Reload()
{
    DestroyShader();
    CreateShader();
}

void ShaderProgram::Bind(CommandBufferHandle _commandBuffer)
{
    if (!name.empty())
        RHI::GetInstance()->BindShaderProgram(_commandBuffer, name);
}

void ShaderProgram::PushVector3(CommandBufferHandle _commandBuffer, const char* _name, void* _data)
{
    if (!name.empty())
    {
        RHI::GetInstance()->PushConstants(_commandBuffer, name, _name, _data, 3 * sizeof(float));
    }}

void ShaderProgram::CreateShader()
{
    std::vector<ShaderSourcePath> sourceAndPaths;
    sourceAndPaths.resize(m_ShaderSources.size());

    for (size_t i = 0; i < m_ShaderSources.size(); i++)
    {
        sourceAndPaths[i].shaderSourceCodePath = m_ShaderSources[i]->path.generic_string();
        sourceAndPaths[i].spvCode = m_ShaderSources[i]->GetShaderSourceFile();
    }

    const ProgramShaderCreateInfo programShaderCreateInfo =
        {
        name,
        m_ShaderInfo
        };
    
    RHI::GetInstance()->CreateShader(programShaderCreateInfo, sourceAndPaths);
}

void ShaderProgram::DestroyShader()
{
    if (!name.empty())
        RHI::GetInstance()->DestroyShader(name);
}
