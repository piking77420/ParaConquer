#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        void CreateImage(VulkanContext* _vulkanContext, uint32_t width, uint32_t height,vk::ImageType imageType, vk::Format _format ,vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, VmaMemoryUsage vmaMemoryUsage, vk::Image* _image, VmaAllocation* _vmaAllocation);

        
    }
    
}
