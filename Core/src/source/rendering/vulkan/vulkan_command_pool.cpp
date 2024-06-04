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

void VulkanCommandPool::BeginSingleCommand()
{
	const VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_CommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(1)
	};
	const VkResult result = vkAllocateCommandBuffers(VulkanInterface::GetDevice().device, &vkCommandBufferAllocateInfo, &m_SingleCommandBuffer);
	VK_CHECK_ERROR(result,"vkCreateCommandPool")
}


void VulkanCommandPool::GetSingleCommandBuffer(VkCommandBuffer* commandBuffer)
{
	*commandBuffer = m_SingleCommandBuffer;
}

void VulkanCommandPool::SubmitSingleCommandBuffer(VkQueue queue)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_SingleCommandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(VulkanInterface::GetDevice().device, m_CommandPool, 1, &m_SingleCommandBuffer);
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
