#pragma once

#include "rendering/vulkan/vulkan_header.h"


BEGIN_PCCORE


class VulkanCommandPool
{
public:
	void Init(const VkDevice& _device, const VkCommandPoolCreateInfo* _vkCommandPoolCreateInfo);

	void Destroy(const VkDevice& _device);

	void AllocCommandBuffer(const VkDevice& device, size_t _nbr, VkCommandBuffer*
	                        _commandBufferPtr) const;
private:
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
};


END_PCCORE