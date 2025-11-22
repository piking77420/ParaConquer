#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiResource.hpp"
#include "LowRenderer/RhiRenderPass.hpp"

namespace Vulkan
{
    void TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image image, vk::Format format,
                               vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags imageAspect,
                               uint32_t _layerCount, uint32_t _levelCount);

    bool HasStencilComponent(vk::Format _format);


    vk::ImageMemoryBarrier ImageBarrierFromResourceState(const PC_CORE::ImageStateTransition& _ImageStateTransition, size_t _FrameIndex);
}
