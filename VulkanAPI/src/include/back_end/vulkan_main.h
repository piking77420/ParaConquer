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
		VULKAN_API VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKAN_API ~VulkanMain();

		VULKAN_API void SwapBuffers();

		VULKAN_API void BeginDraw();

		VULKAN_API void BindProgram(const std::string& _shaderName);
		
		VULKAN_API void Draw();

		VULKAN_API void EnDraw();

		VULKAN_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

		VULKAN_API bool DestroyShader(const std::string&  _shaderName);

		VULKAN_API void WaitDevice();


	private:
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;

		vk::CommandPool m_CommandPool;

		std::array<vk::CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_CommandBuffer;

		// current image index in swapchain
		uint32_t imageIndex = 0;

		// currennt frame inflight
		uint32_t m_CurrentFrame = 0;
		
		VULKAN_API void CreateCommandPool();

		VULKAN_API void CreateCommandBuffer();

	};
}
