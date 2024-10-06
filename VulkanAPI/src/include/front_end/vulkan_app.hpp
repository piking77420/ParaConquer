#pragma once

#include "rendering/render_harware_interface/RHI.hpp"

#include "back_end/vulkan_main.h"

namespace VK_NP
{
	class VulkanApp : public PC_CORE::RHI
	{
	public:
		uint32_t GetCurrentImage() const override;

		VULKAN_API void BeginRender(PC_CORE::CommandPoolHandle _commandBuffer) override;
		
		VULKAN_API void Render() override;

		VULKAN_API void EndRender() override;

		VULKAN_API VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKAN_API ~VulkanApp() override;
		
		VULKAN_API void WaitForAquireImage() override;
		
		VULKAN_API void SwapBuffers(PC_CORE::CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount) override;

		VULKAN_API void WaitDevice() override;

		VULKAN_API void RecreateSwapChain(void* _glfwWindowptr , uint32_t _newWidht, uint32_t _newHeight) override;

		VULKAN_API void PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size) override;
#pragma region Shader
		VULKAN_API void BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName) override;

		VULKAN_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKAN_API bool DestroyShader(const std::string& _shaderProgramName) override;
#pragma endregion Shader
		
		VULKAN_API uint32_t BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage) override;

#pragma region CommandPool Functions
		VULKAN_API void CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
			PC_CORE::CommandPoolHandle* _commandPoolHandle) override;
		
		VULKAN_API void DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle) override;
		
		VULKAN_API void AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle, PC_CORE::CommandBufferHandle* _commandBufferHandle,
			uint32_t _commandBufferAllocationCount) override;

		VULKAN_API void FreeCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle, const PC_CORE::CommandBufferHandle* _commandBufferHandle,
			uint32_t _commandBufferFreeAllocationCount) override;
#pragma endregion CommandPool Functions
	
	private:
		VulkanContext m_VulkanContext;
		
		VulkanBufferMap bufferMap;
		
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;
		
		vk::CommandBuffer m_BindCommandBuffer = VK_NULL_HANDLE;
		
	};



}
