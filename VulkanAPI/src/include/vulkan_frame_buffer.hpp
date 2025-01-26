#pragma once

#include <vulkan_header.h>


namespace Vulkan
{
    
    class VulkanFrameBuffer
    {
    public:

        explicit VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer,  bool _ownBySwapChain = false);
        
        VulkanFrameBuffer() = default;

        ~VulkanFrameBuffer();

        vk::Framebuffer GetFramebuffer() const;
        
    private:
        vk::Framebuffer m_FrameBuffer;

        bool m_OwnBySwapChain;
    };

}
