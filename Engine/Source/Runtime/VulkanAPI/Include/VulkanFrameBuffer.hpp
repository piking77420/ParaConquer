#pragma once

#include <VulkanHeader.h>
#include "LowRenderer/RhiFrameBuffer.hpp"

namespace Vulkan
{
    class VulkanFrameBuffer : public PC_CORE::RhiFrameBuffer
    {
    public:
        bool Build() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &m_FrameBuffers;
        }

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &m_FrameBuffers;
        }

        DEFAULT_COPY_MOVE_OPERATIONS(VulkanFrameBuffer)

        VULKAN_API explicit VulkanFrameBuffer(PC_CORE::Rhi& _Rhi, const std::string& _name, const PC_CORE::CreateFrameInfo& _createFrameInfo);

        VULKAN_API explicit VulkanFrameBuffer(PC_CORE::Rhi& _Rhi, const std::string& _name, vk::Framebuffer _vkFramebuffer, uint32_t _widht, uint32_t _height);

        VULKAN_API ~VulkanFrameBuffer() override;

        VULKAN_API vk::Framebuffer GetFramebuffer() const;

    private:
        std::array<vk::Framebuffer, MaxFramesInFlight> m_FrameBuffers = 
        {
            VK_NULL_HANDLE
        };
    };
}
