#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    void TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
}
