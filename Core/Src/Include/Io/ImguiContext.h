#pragma once

#include "CoreHeader.hpp"
#include "LowRenderer/Rhi.hpp"

#include <Vulkan/Vulkan.hpp>


BEGIN_PCCORE

#define IMGUI_RENDER_DEBUG_COLOR {0,1,0,1}

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

	// TO HANDLE NOT ONLY FRAME INFLIGHT TEXTURE
	PC_CORE_API void CreateImguiVulkanTexture(Texture2D* _texture, VkDescriptorSet* _descriptors, size_t _descriptorsCount);

	PC_CORE_API void DestroyVulkanTexture(VkDescriptorSet* _descriptors, size_t _descriptorsCount);

private:
	vk::DescriptorPool descriptorPool;

	vk::Device device;

	void VulkanInitialize(void* _glfwWindowPtr);
	//static inline PC_CORE::DescriptorPoolHandle m_DescriptorPoolHandle = NULL_HANDLE;
};

END_PCCORE