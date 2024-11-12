#pragma once
#include "vulkan_typedef.h"

namespace Vulkan
{
    namespace Backend
    {
        void TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image _imageHandle,
                                   vk::ImageAspectFlags _imageAspectFlagBits, uint32_t _mipLevel, vk::ImageLayout _initialLayout,
                                   vk::ImageLayout _finalLayout);
    }
}
