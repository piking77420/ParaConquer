#pragma once

#include "vulkan_header.h"


namespace VK_NP
{
    vk::CommandBuffer BeginSingleTimeCommands(vk::Device _device, vk::CommandPool _commandPool);

    void EndSingleTimeCommands(vk::CommandBuffer _commandBuffer);
}


