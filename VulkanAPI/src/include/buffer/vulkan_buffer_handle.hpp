#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "low_renderer/gpu_buffer_handle.hpp"

namespace Vulkan
{
    class VulkanBufferHandle : public PC_CORE::GpuBufferHandle
    {
    public:

        VulkanBufferHandle(VulkanBufferHandle&& _other) noexcept
        {
            buffer = _other.buffer;
            _other.buffer = VK_NULL_HANDLE;

            allocation = _other.allocation;
            _other.allocation = VK_NULL_HANDLE;
        }

        VulkanBufferHandle& operator=(VulkanBufferHandle&& _other)
        {
            buffer = _other.buffer;
            _other.buffer = VK_NULL_HANDLE;

            allocation = _other.allocation;
            _other.allocation = VK_NULL_HANDLE;

            return *this;
        }

        VulkanBufferHandle() = default;

        ~VulkanBufferHandle() override = default;
        
        VkBuffer buffer;
        VmaAllocation allocation;
    };
    
}
