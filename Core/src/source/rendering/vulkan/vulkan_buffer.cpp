#include "rendering/vulkan/vulkan_buffer.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void PC_CORE::VulkanBuffer::Destroy()
{
    if (m_Buffer != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(VulkanInterface::GetAllocator(), m_Buffer, m_Allocation);
    }
    
}

VkBuffer VulkanBuffer::GetHandle()
{
    return m_Buffer;
}

void VulkanBuffer::CreateGpuBuffer(VkBuffer* _vkBuffer, VmaAllocation* _allocation,void const* _data, const size_t _dataSize , VkBufferUsageFlagBits _usage)
{
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingAllocation = VK_NULL_HANDLE;
    
    CreateBufferVma(&stagingBuffer , &stagingAllocation,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY, _dataSize);
    
    void* data;
    vmaMapMemory(VulkanInterface::GetAllocator(), stagingAllocation, &data);
    memcpy(data, _data, _dataSize);
    vmaUnmapMemory(VulkanInterface::GetAllocator(), stagingAllocation);

    CreateBufferVma(_vkBuffer ,_allocation, _usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, _dataSize);

    VulkanInterface::vulkanCommandPoolTransfert.BeginSingleCommand();
    VkCommandBuffer cmdbuffer = {};
    VulkanInterface::vulkanCommandPoolTransfert.GetSingleCommandBuffer(&cmdbuffer);

    VkBufferCopy copy;
    copy.dstOffset = 0;
    copy.srcOffset = 0;
    copy.size = _dataSize;
    vkCmdCopyBuffer(cmdbuffer, stagingBuffer, *_vkBuffer,1,&copy);
    VulkanInterface::vulkanCommandPoolTransfert.SubmitSingleCommandBuffer(VulkanInterface::GetDevice().transferQueue.Queue);

    vmaDestroyBuffer(VulkanInterface::GetAllocator(), stagingBuffer, stagingAllocation);
}

