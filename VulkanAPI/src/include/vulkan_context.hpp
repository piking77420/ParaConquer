#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_descritptor_manager.hpp"
#include "low_renderer/rhi_context.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"

namespace Vulkan
{    
    class VulkanContext : public PC_CORE::RhiContext
    {
    public:
        vk::Queue mainQueue;
        
        vk::Fence transferFence;
        
        vk::CommandPool commandPool = VK_NULL_HANDLE;

        vk::CommandPool transferCommandPool = VK_NULL_HANDLE;

        VmaAllocator allocator = VK_NULL_HANDLE;

        VulkanDescritptorManager descritptorManager;
        
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
        
        VULKAN_API void CreateCommandPools();

        VULKAN_API void WaitIdleInstance() override;
    };


#define GET_VK_INSTANCE \
    std::reinterpret_pointer_cast<VulkanInstance>(VulkanContext::GetContext().renderInstance) \

#define GET_VK_DEVICE \
    std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice) \

#ifdef  DEBUG_GPU_ON
#define SET_VK_DEBUG_NAME(debugInfo)\
    GET_VK_INSTANCE->SetDebugName(GET_VK_DEVICE->GetDevice(), &debugInfo)\

#else
    #define SET_VK_DEBUG_NAME(x)
#endif
}

