#pragma once

#include "vulkan_buffer_map.hpp"
#include "vulkan_command_factory.hpp"
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

		VULKAN_API void PushConstant(const std::string& _shaderName, const char* pushConstantName, const void* _value, size_t _size);

		VULKAN_API bool DestroyShader(const std::string&  _shaderName);
		
		VULKAN_API void WaitDevice();

		VULKAN_API void RecreateSwapChain(void* _glfwWindowPtr , uint32_t _newWidht, uint32_t _newHeight);


	private:
		VulkanContext m_VulkanContext;
		
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;

		VulkanCommandFactory m_SwapChainCommandFactory;

		std::array<vk::CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_CommandBuffer;

		vk::CommandBuffer m_BindCommandBuffer = VK_NULL_HANDLE;

		void BindCommandBuffer(vk::CommandBuffer _commandBuffer);
		

	public:
		// INIT ORDER
		VulkanBufferMap bufferMap;

	};
}
