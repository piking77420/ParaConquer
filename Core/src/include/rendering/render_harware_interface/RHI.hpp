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

	PC_CORE_API virtual ~RHI() {}

	PC_CORE_API virtual uint32_t GetCurrentImage() const
	{
		return 0;
	}

	PC_CORE_API virtual void BeginRender(CommandPoolHandle _commandBuffer)
	{
		
	}

	PC_CORE_API virtual void EndRender()
	{
		
	}

	
	PC_CORE_API virtual void Render()
	{
		
	}
	
	static RHI* const GetInstance(); 
	
	virtual  PC_CORE_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo, const std::vector<ShaderSourceAndPath>& _shaderSource)
	{
		
	}
	
	virtual  PC_CORE_API bool DestroyShader(const std::string& _shaderProgramName)
	{
		return false;
	}

	virtual  PC_CORE_API void BindShaderProgram(CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName)
	{
	}

	virtual  PC_CORE_API void SwapBuffers(CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount)
	{
	}

	virtual PC_CORE_API void WaitForAquireImage()
	{
		
	}

	virtual  PC_CORE_API void WaitDevice()
	{
	}

	virtual  PC_CORE_API void RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
	{
	}

	virtual PC_CORE_API void PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size)
	{
	}

#pragma region CommandPool

	virtual PC_CORE_API void CreateCommandPool(const CommandPoolCreateInfo& _commandPoolCreateInfo, CommandPoolHandle* _commandPoolHandle)
	{

	}

	virtual PC_CORE_API void DestroyCommandPool(CommandPoolHandle _commandPoolHandle)
	{

	}

	virtual PC_CORE_API void AllocCommandBuffers(CommandPoolHandle _commandPoolHandle, CommandBufferHandle* _commandBufferHandle,
		uint32_t _commandBufferAllocationCount)
	{

	}

	virtual PC_CORE_API void FreeCommandBuffers(CommandPoolHandle _commandPoolHandle, const PC_CORE::CommandBufferHandle* _commandBufferHandle,
		uint32_t _commandBufferFreeAllocationCount)
	{

	}

#pragma endregion CommandPool
	
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