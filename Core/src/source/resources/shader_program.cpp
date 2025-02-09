#include "resources/shader_program.h"

#include "math/matrix2x2.hpp"

using namespace PC_CORE;


ShaderProgram::ShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo) : m_ProgramShaderCreateInfo(_programShaderCreateInfo)
{
	name = _programShaderCreateInfo.prograShaderName;
}

