#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        void CreateImage(VulkanContext* _vulkanContext, uint32_t width, uint32_t height, uint32_t mipLevels, vk::ImageType imageType, vk::Format _format ,vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, VmaMemoryUsage vmaMemoryUsage, vk::Image* _image, VmaAllocation* _vmaAllocation);

        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
        {
            
        }   
        
    }
    
}
