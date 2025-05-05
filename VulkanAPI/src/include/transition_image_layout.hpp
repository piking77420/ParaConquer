#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    void TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags imageAspect);

    bool HasStencilComponent(vk::Format _format);

    //void GenerateMipMaps(vk::CommandBuffer _commandBuffer, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _mipLevels);
}
