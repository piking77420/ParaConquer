#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        PC_CORE::RenderPassHandle CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, vk::Device _device);
    }
}
