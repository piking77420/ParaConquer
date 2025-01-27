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

        vk::Queue graphicsQueue;

        vk::Queue presentQueue;
        
        vk::CommandPool commandPool = VK_NULL_HANDLE;
        
        VULKAN_API explicit VulkanContext(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo);

        VULKAN_API ~VulkanContext() override;

        static inline VulkanContext& GetContext()
        {
            return *reinterpret_cast<VulkanContext*>(m_CurrentContext);
        }

        const vk::SurfaceKHR& GetSurface() const { return m_Surface; }

        static std::shared_ptr<VulkanDevice> GetDevice();

        static std::shared_ptr<VulkanPhysicalDevices> GetPhysicalDevices();

    private:
  
        vk::SurfaceKHR m_Surface;
        
        VULKAN_API void InitSurface(const void* _windowHandle);

        VULKAN_API void CreateCommandPool();
    };

}

