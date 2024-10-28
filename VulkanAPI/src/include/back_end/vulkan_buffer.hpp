#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    namespace Backend
    {
        void CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size, VkBufferUsageFlags _vkBufferUsageFlags,
                    VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationCreateFlagBits,
                    vk::Buffer* _buffer, VmaAllocation* _allocation , VmaAllocationInfo* _allocationInfo);

        void CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage,
        vk::Buffer* _vkBuffer, VmaAllocation* _vmaAllocation);

    }
}
