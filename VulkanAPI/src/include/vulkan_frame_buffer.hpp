#pragma once

#include <vulkan_header.h>
#include "low_renderer/frame_buffer.hpp"

namespace Vulkan
{
    
    class VulkanFrameBuffer : public PC_CORE::FrameBuffer
    {
    public:
        
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanFrameBuffer)

        VULKAN_API explicit VulkanFrameBuffer(const PC_CORE::CreateFrameInfo& _createFrameInfo);
        
        VULKAN_API explicit VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer, uint32_t widht, uint32_t height, bool _ownBySwapChain = false);
        
        VULKAN_API VulkanFrameBuffer() = default;

        VULKAN_API ~VulkanFrameBuffer();

        VULKAN_API vk::Framebuffer GetFramebuffer() const;
        
    private:
        std::array<vk::Framebuffer, MAX_FRAMES_IN_FLIGHT> m_FrameBuffers;

        bool m_OwnBySwapChain = false;
    };

}
