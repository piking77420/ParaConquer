#include "resources/shader_program.h"

#include "rendering/RHI.hpp"

using namespace PC_CORE;

ShaderProgram::ShaderProgram(const ProgramShaderCreateInfo& _createInfo, const std::vector<ShaderSource*>& _shaderSources) :
    IResource()
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

void ShaderProgram::Bind()
{
    if (!name.empty())
        RHI::BindShaderProgram(name);
}

void ShaderProgram::CreateShader()
{
    std::vector<ShaderSourceAndPath> sourceAndPaths;
    sourceAndPaths.resize(m_ShaderSources.size());

    for (size_t i = 0; i < m_ShaderSources.size(); i++)
    {
        sourceAndPaths[i].shaderSourceCodePath = m_ShaderSources[i]->path.generic_string().c_str();
        sourceAndPaths[i].shaderSourceCode = m_ShaderSources[i]->GetShaderSourceFile();
    }

    const ProgramShaderCreateInfo programShaderCreateInfo =
        {
        name,
        m_ShaderInfo
        };
    RHI::CreateShader(programShaderCreateInfo, sourceAndPaths);

}

void ShaderProgram::DestroyShader()
{
    if (!name.empty())
        RHI::DestroyShader(name);
}
