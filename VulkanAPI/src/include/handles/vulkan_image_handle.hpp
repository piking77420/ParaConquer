#pragma once

#include <vma/vk_mem_alloc.h>
#include "vulkan_header.h"
#include "low_renderer/gpu_handle.hpp"

namespace Vulkan
{
    class VulkanImageHandle : public PC_CORE::GpuHandle
    {
    public:
    
        VulkanImageHandle(VulkanImageHandle&& _other) noexcept
        {
            image = _other.image;
            _other.image = VK_NULL_HANDLE;

            allocation = _other.allocation;
            _other.allocation = VK_NULL_HANDLE;
        }

        VulkanImageHandle& operator=(VulkanImageHandle&& _other)
        {
            image = _other.image;
            _other.image = VK_NULL_HANDLE;

            allocation = _other.allocation;
            _other.allocation = VK_NULL_HANDLE;

            return *this;
        }

        VulkanImageHandle() = default;

        ~VulkanImageHandle() override = default;
        
        VkImage image;
        VmaAllocation allocation;
    };

}

