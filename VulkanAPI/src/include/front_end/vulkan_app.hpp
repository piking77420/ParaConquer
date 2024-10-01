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

		VULKAN_API void InstanceSwapBuffers() override;

		VULKAN_API void InstanceBeginRender() override;
		
		VULKAN_API void InstanceEndRender() override;

		VULKAN_API void InstanceRender() override;	

		VULKAN_API void InstanceBindShaderProgram(const std::string& _shaderProgramName) override;

		VULKAN_API void InstanceRecreateSwapChain(void* _glfwWindowptr , uint32_t _newWidht, uint32_t _newHeight) override;

		VULKAN_API void InstancePushConstant(const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size) override;
	
	private:
		VK_NP::VulkanMain m_VulkanMain;

		VULKAN_API void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKAN_API bool InstanceDestroyShader(const std::string& _shaderProgramName) override;

		VULKAN_API void InstanceWaitDevice() override;

		VULKAN_API uint32_t InstanceBufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage) override;

	};



}
