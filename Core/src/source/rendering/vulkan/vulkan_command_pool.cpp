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
	VkResult result = vkAllocateCommandBuffers(VulkanInterface::GetDevice().device, &vkCommandBufferAllocateInfo, &m_SingleCommandBuffer);
	VK_CHECK_ERROR(result,"vkCreateCommandPool")


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	result = vkBeginCommandBuffer(m_SingleCommandBuffer, &beginInfo);
	VK_CHECK_ERROR(result,"vkBeginCommandBuffer BeginSingleCommand")

}


void VulkanCommandPool::GetSingleCommandBuffer(VkCommandBuffer* commandBuffer)
{
	*commandBuffer = m_SingleCommandBuffer;
}

void VulkanCommandPool::SubmitSingleCommandBuffer(VkQueue queue)
{
	VkResult result = vkEndCommandBuffer(m_SingleCommandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_SingleCommandBuffer;

	result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	VK_CHECK_ERROR(result,"vkQueueSubmit SubmitSingleCommandBuffer")

	result =  vkQueueWaitIdle(queue);
	VK_CHECK_ERROR(result,"vkQueueWaitIdle(queue) SubmitSingleCommandBuffer")

	FreeCommandBuffers(1,&m_SingleCommandBuffer);
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
