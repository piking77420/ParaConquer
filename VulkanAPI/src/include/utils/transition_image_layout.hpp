#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    void TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags imageAspect, uint32_t _layerCount, uint32_t _levelCount);

    bool HasStencilComponent(vk::Format _format);

}
