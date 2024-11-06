#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        void CreateBuffer(VulkanContext* _vulkanContext, size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage,
                vk::Buffer* _outBuffer, VmaAllocation* _outVmaAllocation);
        
        void CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size, vk::BufferUsageFlags _vkBufferUsageFlags,
                    VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationCreateFlagBits,
                    vk::Buffer* _buffer, VmaAllocation* _allocation , VmaAllocationInfo* _allocationInfo);

        void CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* _data, vk::BufferUsageFlags _vkBufferUsageFlags,
        vk::Buffer* _vkBuffer, VmaAllocation* _vmaAllocation);

    }
}
