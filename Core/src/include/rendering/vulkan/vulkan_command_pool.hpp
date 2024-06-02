#pragma once

#include "rendering/vulkan/vulkan_header.h"


BEGIN_PCCORE


class VulkanCommandPool
{
public:
	void Init(const VkCommandPoolCreateInfo* _vkCommandPoolCreateInfo);

	void Destroy();

	void AllocCommandBuffer(size_t _nbr, VkCommandBuffer*
	                        _commandBufferPtr) const;

	void FreeCommandBuffers(size_t _nbr, VkCommandBuffer* _vkCommandBuffer);
private:
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
};


END_PCCORE