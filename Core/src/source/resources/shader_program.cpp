#include "resources/shader_program.h"

#include "low_renderer/rhi.hpp"
#include "math/matrix2x2.hpp"

using namespace PC_CORE;


ShaderProgram::ShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo) : m_ProgramShaderCreateInfo(_programShaderCreateInfo)
{
	name = _programShaderCreateInfo.prograShaderName;
}

ShaderProgram::~ShaderProgram()
{
	//Rhi::DestroyShader(this);
}

