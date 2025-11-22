#pragma once

#include <VulkanHeader.h>
#include "LowRenderer/FrameBuffer.hpp"

namespace Vulkan
{
    class VulkanFrameBuffer : public PC_CORE::FrameBuffer
    {
    public:
        VULKAN_API const void* GetNativeHandle() const override
        {
            return &m_FrameBuffers;
        }

        VULKAN_API void* GetNativeHandle() override
        {
            return &m_FrameBuffers;
        }

        DEFAULT_COPY_MOVE_OPERATIONS(VulkanFrameBuffer)

        VULKAN_API explicit VulkanFrameBuffer(const PC_CORE::CreateFrameInfo& _createFrameInfo);

        VULKAN_API explicit VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer, uint32_t widht, uint32_t height);

        VULKAN_API VulkanFrameBuffer() = default;

        VULKAN_API ~VulkanFrameBuffer() override;

        VULKAN_API vk::Framebuffer GetFramebuffer() const;

    private:
        std::array<vk::Framebuffer, MaxFramesInFlight> m_FrameBuffers;
    };
}
