#pragma once

#include <string>
#include <vector>

#include "rhi_typedef.h"

BEGIN_PCCORE

class RHI
{
public:
	PC_CORE_API static void MakeInstance(RHI* _rhi);

	PC_CORE_API static void DestroyInstance();

	PC_CORE_API virtual ~RHI() {};
	
	PC_CORE_API virtual void SwapBuffers() {};

	static PC_CORE_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo, const std::vector<ShaderSourceAndPath>& _shaderSource)
	{
		m_Instance->InstanceCreateShader(_programShaderCreateInfo, _shaderSource);
	}

	static PC_CORE_API bool DestroyShader(const std::string& _shaderProgramName)
	{
		return m_Instance->InstanceDestroyShader(_shaderProgramName);
	}

protected:
	static RHI* m_Instance;

	PC_CORE_API virtual void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>&
	                                                      _shaderSource) = 0;

	PC_CORE_API virtual bool InstanceDestroyShader(const std::string& _shaderProgramName) = 0;
};

END_PCCORE