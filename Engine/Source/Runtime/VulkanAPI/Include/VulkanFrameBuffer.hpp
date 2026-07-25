#pragma once

#include <VulkanHeader.h>
#include "LowRenderer/RhiFrameBuffer.hpp"

namespace Vulkan
{
    class VulkanFrameBuffer : public PC_CORE::RhiFrameBuffer
    {
    public:
        VULKAN_API explicit VulkanFrameBuffer(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanFrameBuffer() override;

        bool Build() override;        

        VULKAN_API vk::Framebuffer GetVkFramebuffer() const;


    private:
        vk::Framebuffer m_FrameBufferHandle = VK_NULL_HANDLE;

        std::vector<vk::ImageView> m_ImageViews;
    };
}
