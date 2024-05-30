#include "rendering/vulkan/vulkan_command_pool.hpp"

#include <string>

#include "log.hpp"

using namespace PC_CORE;

void VulkanCommandPool::Init(const VkDevice& _device, const VkCommandPoolCreateInfo* _vkCommandPoolCreateInfo)
{
	const VkResult result = vkCreateCommandPool(_device, _vkCommandPoolCreateInfo, nullptr, &m_CommandPool);

	VK_CHECK_ERROR(result,"vkCreateCommandPool")

	Log::Debug("Command buffer pool created");
}

void VulkanCommandPool::Destroy(const VkDevice& _device)
{
	vkDestroyCommandPool(_device, m_CommandPool, nullptr);
}

void VulkanCommandPool::AllocCommandBuffer(const VkDevice& device, size_t _nbr, VkCommandBuffer* _commandBufferPtr) const
{
	const VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_CommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(_nbr)
	};
	const VkResult result = vkAllocateCommandBuffers(device, &vkCommandBufferAllocateInfo, _commandBufferPtr);

	VK_CHECK_ERROR(result,"vkCreateCommandPool")

	Log::Debug("Command buffer pool created");
}
