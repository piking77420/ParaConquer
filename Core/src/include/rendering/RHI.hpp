#pragma once

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

	static PC_CORE_API bool CompileShaderSource(const std::vector<ShaderSourceAndPath>& _shaderSource)
	{
		return m_Instance->InstanceCompileShadersSource(_shaderSource);
	}

	static PC_CORE_API bool DestroyShader(uint32_t _shaderHandle)
	{
		return m_Instance->InstanceDestroyShader(_shaderHandle);
	}

private:
	static RHI* m_Instance;

	PC_CORE_API virtual bool InstanceCompileShadersSource(const std::vector<ShaderSourceAndPath>& _shaderSource) = 0;

	PC_CORE_API virtual bool InstanceDestroyShader(uint32_t _shaderHandle) = 0;
};

END_PCCORE