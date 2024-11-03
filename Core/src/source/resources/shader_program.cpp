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
        RHI::GetInstance().BindShaderProgram(_commandBuffer, name);
}

void ShaderProgram::PushConstantMat4(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Matrix4x4f& m4)
{
    PushConstant(_commandBuffer, _pushConstantName, m4.GetPtr() , sizeof(Tbx::Matrix4x4f));
}

void ShaderProgram::PushConstantVec3(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Vector3f& vec3)
{
    PushConstant(_commandBuffer, _pushConstantName, vec3.GetPtr() , sizeof(Tbx::Vector3f));
}

PC_CORE::DescriptorSetHandle* ShaderProgram::GetDescriptorSets() const
{
    return RHI::GetInstance().GetDescriptorSet(name);
}

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
    
    RHI::GetInstance().CreateShader(programShaderCreateInfo, sourceAndPaths);
}

void ShaderProgram::DestroyShader()
{
    if (!name.empty())
        RHI::GetInstance().DestroyShader(name);
}

void ShaderProgram::PushConstant(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const void* _data, size_t _dataSize)
{
    if (!name.empty())
    {
        RHI::GetInstance().PushConstants(_commandBuffer, name, _pushConstantName, _data, _dataSize);
    }
}
