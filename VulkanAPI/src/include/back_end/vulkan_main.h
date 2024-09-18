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

		VULKA_API void BeginDraw();

		VULKA_API void BindProgram(const std::string& _shaderName);
		
		VULKA_API void Draw();

		VULKA_API void EnDraw();

		VULKA_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

		VULKA_API bool DestroyShader(const std::string&  _shaderName);


	private:
	
		
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;

		vk::CommandPool m_CommandPool;

		vk::CommandBuffer m_CommandBuffer;
		
		uint32_t imageIndex = 0;

		VULKA_API void CreateCommandPool();

		VULKA_API void CreateCommandBuffer();

	};
}
