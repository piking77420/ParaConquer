#pragma once

#include <VulkanHeader.h>
#include "LowRenderer/RhiFrameBuffer.hpp"

namespace Vulkan
{
    class VulkanFrameBuffer : public PC_CORE::RhiFrameBuffer
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanFrameBuffer)

        VULKAN_API explicit VulkanFrameBuffer(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanFrameBuffer() override;

        bool Build() override;        

        VULKAN_API vk::Framebuffer GetVkFramebuffer() const;

        std::array<vk::Framebuffer, MaxFramesInFlight> FrameBuffers =
        {
            VK_NULL_HANDLE
        };

    private:
        
    };
}
