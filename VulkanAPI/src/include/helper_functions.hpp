#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    struct SingleCommandBeginInfo
    {
        vk::Device device;
        vk::CommandPool commandPool;
        vk::Queue queue;
    };
    
    vk::CommandBuffer BeginSingleTimeCommand(const SingleCommandBeginInfo& _singleCommandBeginInfo);

    void EndSingleTimeCommand(vk::CommandBuffer _commandBuffer, const SingleCommandBeginInfo& _singleCommandBeginInfo);
    
}
