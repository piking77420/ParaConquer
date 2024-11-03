#pragma once
#include "vulkan_typedef.h"

namespace Vulkan
{
    namespace Backend
    {
        void TransitionImageLayout(VulkanContext* _context, vk::Image _imageHandle,
                                   vk::ImageAspectFlagBits _imageAspectFlagBits, uint32_t _mipLevel, vk::ImageLayout _initialLayout,
                                   vk::ImageLayout _finalLayout);
    }
}
