#include "rendering/vulkan/vulkan_buffer.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_command_buffer.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void PC_CORE::VulkanBuffer::Destroy()
{
    if (m_Buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(VulkanInterface::GetDevice().device, m_Buffer, nullptr);
}

VkBuffer VulkanBuffer::GetHandle()
{
    return m_Buffer;
}

void VulkanBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{

    const VkDevice device = VulkanInterface::GetDevice().device;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
    VK_CHECK_ERROR(result,"failed to create buffer!");
    

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanInterface::FindMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
    VK_CHECK_ERROR(result,"failed to allocate buffer memory!");

    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

void VulkanBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, size_t _srcOffset,
    size_t _destOffset)
{
    VulkanCommandBuffer vulkanCommandBuffer;
    vulkanCommandBuffer.SetNbrofAllocation(1);

    VulkanInterface::instance->vulkanCommandPoolTransfert.AllocCommandBuffer(1,vulkanCommandBuffer.GetPtr());

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(vulkanCommandBuffer[0], &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = _srcOffset; // Optional
    copyRegion.dstOffset = _destOffset; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(vulkanCommandBuffer[0], srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(vulkanCommandBuffer[0]);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vulkanCommandBuffer[0];

    vkQueueSubmit(VulkanInterface::GetDevice().transferQueue.Queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(VulkanInterface::GetDevice().transferQueue.Queue);

    VulkanInterface::instance->vulkanCommandPoolTransfert.FreeCommandBuffers(1,vulkanCommandBuffer.GetPtr());
}
