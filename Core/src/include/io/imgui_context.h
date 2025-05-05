#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi.hpp"

#include <vulkan/vulkan.hpp>


BEGIN_PCCORE
	class  IMGUIContext
{
public:
	PC_CORE_API void Init(void* _glfwWindowPtr, PC_CORE::GraphicAPI _graphicApi);
	
	PC_CORE_API void NewFrame();

	PC_CORE_API void Destroy();

	//PC_CORE_API static void Render(PC_CORE::CommandBuffer _commandBuffer);
	
	PC_CORE_API IMGUIContext() = default;

	PC_CORE_API ~IMGUIContext() = default;

	PC_CORE_API void Render(CommandList* _commandBuffer);

	PC_CORE_API void CreateImguiVulkanViewport(Texture* _texture, std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& _viewPortId);

	PC_CORE_API void RemoveImguiVulkanViewport(std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& _viewPortId);

private:
	vk::DescriptorPool descriptorPool;

	vk::Device device;

	void VulkanInitialize(void* _glfwWindowPtr);
	//static inline PC_CORE::DescriptorPoolHandle m_DescriptorPoolHandle = NULL_HANDLE;
};

END_PCCORE