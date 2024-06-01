#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanBuffer
{
public:
    void Destroy();

    VulkanBuffer() = default;

    virtual ~VulkanBuffer() = default;

    VkBuffer GetHandle();
    
protected:
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

    static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,size_t _srcOffset = 0,
    size_t _destOffset = 0);
};

END_PCCORE