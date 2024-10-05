#pragma once

#include "rendering/RHI.hpp"

#include "back_end/vulkan_main.h"

namespace VK_NP
{
	class VulkanApp : public PC_CORE::RHI
	{
	public:
		
		VULKAN_API VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKAN_API ~VulkanApp() override;

		VULKAN_API void SwapBuffers() override;

		VULKAN_API void BeginRender() override;
		
		VULKAN_API void EndRender() override;

		VULKAN_API void Render() override;	

		VULKAN_API void BindShaderProgram(const std::string& _shaderProgramName) override;

		VULKAN_API void RecreateSwapChain(void* _glfwWindowptr , uint32_t _newWidht, uint32_t _newHeight) override;

		VULKAN_API void PushConstants(const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size) override;
	
	private:
		VK_NP::VulkanMain m_VulkanMain;

		VULKAN_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKAN_API bool DestroyShader(const std::string& _shaderProgramName) override;

		VULKAN_API void WaitDevice() override;

		VULKAN_API uint32_t BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage) override;

	};



}
