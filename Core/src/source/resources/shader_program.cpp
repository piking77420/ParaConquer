#include "resources/shader_program.h"

#include "low_renderer/rhi.hpp"
#include "math/matrix2x2.hpp"

using namespace PC_CORE;


void ShaderProgram::AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set)
{
    m_ShaderProgram->AllocDescriptorSet(_shaderProgramDescriptorSets, set);
}

void ShaderProgram::FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets)
{
    m_ShaderProgram->FreeDescriptorSet(_shaderProgramDescriptorSets);
}

ShaderProgram::ShaderProgram(const std::string& _shaderName, const ProgramShaderCreateInfo& _programShaderCreateInfo) : Resource(_shaderName)
{
    DYNAMIC_REFLECT_INIT
    
    m_ShaderProgram = Rhi::CreateRhiShaderProgram(_programShaderCreateInfo);
}

