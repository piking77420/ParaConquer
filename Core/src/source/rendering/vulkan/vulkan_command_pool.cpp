#include "rendering/vulkan/vulkan_command_pool.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanCommandPool::Init(const VkCommandPoolCreateInfo* _vkCommandPoolCreateInfo)
{
	const VkDevice& device = VulkanInterface::GetDevice().device;

	const VkResult result = vkCreateCommandPool(device, _vkCommandPoolCreateInfo, nullptr, &m_CommandPool);

	VK_CHECK_ERROR(result,"vkCreateCommandPool")

	Log::Debug("Command buffer pool created");
}

void VulkanCommandPool::Destroy()
{
	const VkDevice& device = VulkanInterface::GetDevice().device;

	vkDestroyCommandPool(device, m_CommandPool, nullptr);
}

void VulkanCommandPool::AllocCommandBuffer(size_t _nbr, VkCommandBuffer* _commandBufferPtr) const
{
	const VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_CommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(_nbr)
	};
	const VkResult result = vkAllocateCommandBuffers(VulkanInterface::GetDevice().device, &vkCommandBufferAllocateInfo, _commandBufferPtr);

	VK_CHECK_ERROR(result,"vkCreateCommandPool")

	Log::Debug("Command buffer pool created");
}

void VulkanCommandPool::FreeCommandBuffers(size_t _nbr, VkCommandBuffer* _vkCommandBuffer)
{
	vkFreeCommandBuffers(VulkanInterface::GetDevice().device, m_CommandPool, static_cast<uint32_t>(_nbr), _vkCommandBuffer);
}
