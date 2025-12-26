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

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &FrameBuffers;
        }

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &FrameBuffers;
        }

        

        VULKAN_API vk::Framebuffer GetFramebuffer() const;

        std::array<vk::Framebuffer, MaxFramesInFlight> FrameBuffers =
        {
            VK_NULL_HANDLE
        };

    private:
        
    };
}
