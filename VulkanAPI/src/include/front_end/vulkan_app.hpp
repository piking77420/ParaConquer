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

		
	private:
		VK_NP::VulkanMain m_VulkanMain;

		VULKAN_API void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKAN_API bool InstanceDestroyShader(const std::string& _shaderProgramName) override;

		VULKAN_API void InstanceWaitDevice() override;

	};



}
