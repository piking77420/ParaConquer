#pragma once

#include "vulkan_harware_wrapper.hpp"
#include "vulkan_header.h"
#include "vulkan_present_chain.hpp"
#include "vulkan_shader_manager.hpp"

namespace VK_NP
{

	class VulkanMain
	{
	public:
		VULKA_API VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKA_API ~VulkanMain();

		VULKA_API void SwapBuffers();

		VULKA_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

		VULKA_API bool DestroyShader(const std::string&  _shaderName);

	private:
		VulkanHarwareWrapper m_vulkanHarwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		//VulkanShaderManager m_vulkanShaderManager;
		
	};
}
