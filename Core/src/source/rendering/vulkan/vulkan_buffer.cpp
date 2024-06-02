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

void VulkanBuffer::CreateBuffer(VkBuffer* _vkBuffer, VmaAllocation* _allocation,
    VkBufferUsageFlags _vkBufferUsageFlags, VmaMemoryUsage _vmaMemoryUsage, void const* _data, size_t _size)
{
    //allocate vertex buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //this is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = _size;
    //this buffer is going to be used as a Vertex Buffer
    bufferInfo.usage = _vkBufferUsageFlags;
	
    //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = _vmaMemoryUsage;
    
    //allocate the buffer
    VkResult result = vmaCreateBuffer(VulkanInterface::GetAllocator(), &bufferInfo, &vmaallocInfo, _vkBuffer, _allocation, nullptr);
    VK_CHECK_ERROR(result,"Failded to createBuffer");
    
    void* data;
    vmaMapMemory(VulkanInterface::GetAllocator(), *_allocation, &data);

    memcpy(data, _data, bufferInfo.size);

    vmaUnmapMemory(VulkanInterface::GetAllocator(), *_allocation);
}

