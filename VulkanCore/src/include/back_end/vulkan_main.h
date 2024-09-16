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

		VULKA_API ~VulkanMain() = default;

		VULKA_API void SwapBuffers();

		VULKA_API uint32_t CreateShader(const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

		VULKA_API bool DestroyShader(uint32_t _shaderHandle);

	private:
		VulkanHarwareWrapper m_vulkanHarwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;
	};
}
