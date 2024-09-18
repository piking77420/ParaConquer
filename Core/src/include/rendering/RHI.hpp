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
	

	static PC_CORE_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo, const std::vector<ShaderSourceAndPath>& _shaderSource)
	{
		m_Instance->InstanceCreateShader(_programShaderCreateInfo, _shaderSource);
	}

	static PC_CORE_API bool DestroyShader(const std::string& _shaderProgramName)
	{
		return m_Instance->InstanceDestroyShader(_shaderProgramName);
	}

	static PC_CORE_API void BindShaderProgram(const std::string& _shaderProgramName)
	{
		m_Instance->InstanceBindShaderProgram(_shaderProgramName);
	}

	static PC_CORE_API void SwapBuffers()
	{
		m_Instance->InstanceSwapBuffers();
	}

	static PC_CORE_API void BeginDraw()
	{
		m_Instance->InstanceBeginRender();
	}

	static PC_CORE_API void Render()
	{
		m_Instance->InstanceRender();
	}
	
	static PC_CORE_API void EndDraw()
	{
		m_Instance->InstanceEndRender();
	}

	static PC_CORE_API void WaitDevice()
	{
		m_Instance->InstanceWaitDevice();
	}


protected:
	static RHI* m_Instance;

	PC_CORE_API virtual void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>&
	                                                      _shaderSource) = 0;

	PC_CORE_API virtual bool InstanceDestroyShader(const std::string& _shaderProgramName) = 0;

	PC_CORE_API virtual  void InstanceBeginRender() {};
	
	PC_CORE_API virtual  void InstanceRender() {};
	
	PC_CORE_API virtual  void InstanceEndRender() {};

	PC_CORE_API virtual void InstanceSwapBuffers() {};

	PC_CORE_API virtual void InstanceBindShaderProgram(const std::string& _shaderProgramName) {};


	PC_CORE_API virtual void InstanceWaitDevice() {};

	
};

END_PCCORE