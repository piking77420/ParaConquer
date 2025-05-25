#pragma once

#include <vma/vk_mem_alloc.h>
#include "vulkan_header.h"
#include "low_renderer/gpu_resource.hpp"

namespace Vulkan
{
    class VulkanImageHandle : public PC_CORE::GPUResource
    {
    public:
    
        VulkanImageHandle(VulkanImageHandle&& _other) noexcept
        {
            std::swap(image, _other.image);

            std::swap(view, _other.view);
            
            std::swap(allocation, _other.allocation);
        }

        VulkanImageHandle& operator=(VulkanImageHandle&& _other) noexcept
        {
            std::swap(image, _other.image);

            std::swap(view, _other.view);
            
            std::swap(allocation, _other.allocation);

            return *this;
        }

        VULKAN_API VulkanImageHandle();

        VULKAN_API ~VulkanImageHandle() override;

        VULKAN_API void Clear() override;
        
        VkImage image;
        VkImageView view;
        VmaAllocation allocation;

    };

}

