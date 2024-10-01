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

	static PC_CORE_API void RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
	{
		m_Instance->InstanceRecreateSwapChain(_glfwWindowptr, _newWidht, _newHeight);
	}

	static PC_CORE_API void PushConstants(const std::string& _shaderProgramName, const std::string& _pushConstantsName,
		const void* _data, uint32_t _size)
	{
		m_Instance->InstancePushConstant(_shaderProgramName, _pushConstantsName, _data, _size);
	}

	
#pragma region Buffer

	static PC_CORE_API uint32_t BufferData(size_t _size, const void* _data, GPU_BUFFER_USAGE _usage)
	{
		return m_Instance->InstanceBufferData(_size, _data, _usage);
	}

#pragma endregion	Buffer


protected:
	static RHI* m_Instance;

	PC_CORE_API virtual void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>&
	                                                      _shaderSource) = 0;
	
	PC_CORE_API virtual bool InstanceDestroyShader(const std::string& _shaderProgramName) = 0;

	PC_CORE_API virtual void InstancePushConstant(const std::string& _shaderProgramName, const std::string& _pushConstantsName, 
		const void* _data, uint32_t _size) = 0;
	
	PC_CORE_API virtual  void InstanceBeginRender() {};
	
	PC_CORE_API virtual  void InstanceRender() {};
	
	PC_CORE_API virtual  void InstanceEndRender() {};

	PC_CORE_API virtual void InstanceSwapBuffers() {};

	PC_CORE_API virtual void InstanceBindShaderProgram(const std::string& _shaderProgramName) {};
	
	PC_CORE_API virtual void InstanceWaitDevice() {};

	PC_CORE_API virtual void InstanceRecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight) {};

	PC_CORE_API virtual uint32_t InstanceBufferData(size_t _size, const void* _data, GPU_BUFFER_USAGE _usage) { return static_cast<uint32_t>(-1);}

};

END_PCCORE