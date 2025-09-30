#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiTypedef.h"
#include <vma/vk_mem_alloc.h>

namespace Vulkan
{
    namespace Utils
    {
        VULKAN_API  void CreateBuffer(VmaAllocator allocator, size_t size, vk::BufferUsageFlags _bufferUsageFlagBits, VmaMemoryUsage _memoryUsage,
             VkBuffer* _outBuffer, VmaAllocation* _outAllocation);

        VULKAN_API  void DestroyBuffer(vk::Device device , VmaAllocator allocator,
            VkBuffer buffer, VmaAllocation allocation);
        
    }
    
}
