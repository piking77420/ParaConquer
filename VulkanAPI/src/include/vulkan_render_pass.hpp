#pragma once

#include "vulkan_header.h"
#include "low_renderer/rhi_render_pass.hpp"

namespace Vulkan
{
    class VulkanRenderPass : public PC_CORE::RhiRenderPass
    {
    public:
        VULKAN_API VulkanRenderPass(vk::SurfaceFormatKHR surfaceFormat);

        VULKAN_API VulkanRenderPass() = default;

        VULKAN_API ~VulkanRenderPass() override;

        VULKAN_API vk::RenderPass GetVulkanRenderPass() const;
        
        
    protected:
        vk::RenderPass m_RenderPass = VK_NULL_HANDLE;
    };
    
}

