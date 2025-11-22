#pragma once
#include "VulkanHeader.h"

namespace Vulkan::Utils
{
    struct SingleCommandBeginInfo
    {
        vk::Device device;
        vk::CommandPool commandPool;
        vk::Queue queue;
    };

    vk::CommandBuffer BeginSingleTimeCommand(const SingleCommandBeginInfo& _singleCommandBeginInfo);

    void EndSingleTimeCommand(vk::CommandBuffer _commandBuffer, const SingleCommandBeginInfo& _singleCommandBeginInfo,
                              vk::Fence _fence);
}
