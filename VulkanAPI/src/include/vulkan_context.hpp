#pragma once

#include "low_renderer/rhi_context.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"
#include "vulkan_gpu_allocator.hpp"

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

        VULKAN_API const vk::SurfaceKHR& GetSurface() const { return std::reinterpret_pointer_cast<VulkanInstance>(renderInstance)->surface; }

        VULKAN_API static std::shared_ptr<VulkanDevice> GetDevice();

        VULKAN_API static std::shared_ptr<VulkanPhysicalDevices> GetPhysicalDevices();
    
    private:

        VULKAN_API void CreateMemoryAllocator();
        
        VULKAN_API void CreateCommandPool();

        VULKAN_API void WaitIdleInstance() override;
    };

}

