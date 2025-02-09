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
            std::exchange(image, _other.image);

            std::exchange(view, _other.view);
            
            std::exchange(allocation, _other.allocation);
        }

        VulkanImageHandle& operator=(VulkanImageHandle&& _other)
        {
            std::exchange(image, _other.image);

            std::exchange(view, _other.view);
            
            std::exchange(allocation, _other.allocation);

            return *this;
        }

        VulkanImageHandle() = default;

        ~VulkanImageHandle() override = default;
        
        VkImage image;
        VkImageView view;
        VmaAllocation allocation;
    };

}

