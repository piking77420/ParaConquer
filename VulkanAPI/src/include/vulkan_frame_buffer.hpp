#pragma once

#include <vulkan_header.h>
#include "low_renderer/frame_buffer.hpp"

namespace Vulkan
{
    
    class VulkanFrameBuffer : public PC_CORE::FrameBuffer
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(VulkanFrameBuffer)

        VULKAN_API explicit VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer,  bool _ownBySwapChain = false);
        
        VULKAN_API VulkanFrameBuffer() = default;

        VULKAN_API ~VulkanFrameBuffer();

        VULKAN_API vk::Framebuffer GetFramebuffer() const;
        
    private:
        vk::Framebuffer m_FrameBuffer;

        bool m_OwnBySwapChain;
    };

}
