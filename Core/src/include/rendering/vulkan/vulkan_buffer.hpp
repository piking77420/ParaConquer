#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"

BEGIN_PCCORE
class VulkanBuffer
{
public:
    static void CreateGpuBuffer(VkBuffer* _vkBuffer, VmaAllocation* _allocation, void const* _data, const size_t _dataSize, VkBufferUsageFlagBits _usage);
    
    static void CreateGpuBuffer(VkBuffer* _vkBuffer, VmaAllocation* _allocation, const size_t _dataSize, VkBufferUsageFlagBits _usage);

    virtual void Destroy();

    VulkanBuffer() = default;

    virtual ~VulkanBuffer() = default;

    VkBuffer& GetHandle();

    
protected:
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VmaAllocation m_Allocation = VK_NULL_HANDLE;


};

END_PCCORE