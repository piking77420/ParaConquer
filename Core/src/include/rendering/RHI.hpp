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
	
	static RHI* const GetInstance(); 
	
	virtual  PC_CORE_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo, const std::vector<ShaderSourceAndPath>& _shaderSource)
	{
		
	}
	
	virtual  PC_CORE_API bool DestroyShader(const std::string& _shaderProgramName)
	{
		return false;
	}

	virtual  PC_CORE_API void BindShaderProgram(const std::string& _shaderProgramName)
	{
	}

	virtual  PC_CORE_API void SwapBuffers()
	{
	}

	virtual  PC_CORE_API void BeginRender()
	{
	}

	virtual  PC_CORE_API void Render()
	{
	}
	
	virtual  PC_CORE_API void EndRender()
	{
	}

	virtual  PC_CORE_API void WaitDevice()
	{
	}

	virtual  PC_CORE_API void RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
	{
	}

	virtual  PC_CORE_API void PushConstants(const std::string& _shaderProgramName, const std::string& _pushConstantsName,
		const void* _data, uint32_t _size)
	{
	}


	
#pragma region Buffer

	virtual PC_CORE_API uint32_t BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage)
	{
		return -1;
	}

#pragma endregion	Buffer


protected:
	static RHI* m_Instance;

};

END_PCCORE