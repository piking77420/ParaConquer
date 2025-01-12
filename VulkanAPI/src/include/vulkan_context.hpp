#pragma once

#include "low_renderer/rhi_context.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"

namespace Vulkan
{

    constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    
    class VulkanContext : public PC_CORE::RhiContext
    {
    public:
        VULKAN_API explicit VulkanContext(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo);

        VULKAN_API ~VulkanContext() override;

        const vk::SurfaceKHR& GetSurface() const { return m_Surface; }

    private:
        vk::Queue m_GraphicsQueue;

        vk::Queue m_PresentQueue;

        vk::SurfaceKHR m_Surface;
        
        VULKAN_API void InitSurface(const void* _windowHandle);
    };

}

