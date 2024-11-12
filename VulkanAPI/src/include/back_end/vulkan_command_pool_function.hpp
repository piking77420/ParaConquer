#pragma once

#include "vulkan_header.h"


namespace Vulkan
{
    vk::CommandBuffer BeginSingleTimeCommands(vk::Device _device, vk::CommandPool _commandPool);

    void EndSingleTimeCommands(vk::CommandBuffer _commandBuffer,vk::Device _device, vk::Fence _fence, vk::Queue _queue);
}


