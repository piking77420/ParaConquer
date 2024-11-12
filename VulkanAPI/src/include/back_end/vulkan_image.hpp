#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        void CreateImage(VulkanContext* _vulkanContext, uint32_t width, uint32_t height, uint32_t mipLevels, vk::ImageType imageType, vk::Format _format ,vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, VmaMemoryUsage vmaMemoryUsage, vk::Image* _image, VmaAllocation* _vmaAllocation);

        void GenerateMipmaps(vk::Image _image, vk::Format _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t mipLevels,
            vk::PhysicalDevice _physicalDevice, vk::Device _device ,vk::CommandPool _commandPool, vk::Fence _fence, vk::Queue _queue);

    }
    
}
