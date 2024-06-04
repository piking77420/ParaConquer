#pragma once

#include "rendering/vulkan/vulkan_header.h"


BEGIN_PCCORE


class VulkanCommandPool
{
public:
	void Init(const VkCommandPoolCreateInfo* _vkCommandPoolCreateInfo);

	void Destroy();

	void BeginSingleCommand();

	void GetSingleCommandBuffer(VkCommandBuffer* commandBuffer);

	void SubmitSingleCommandBuffer(VkQueue queue);
	
	void AllocCommandBuffer(size_t _nbr, VkCommandBuffer*
	                        _commandBufferPtr) const;

	void FreeCommandBuffers(size_t _nbr, VkCommandBuffer* _commandBufferPtr);
	
private:
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;

	VkCommandBuffer m_SingleCommandBuffer = VK_NULL_HANDLE;
};


END_PCCORE