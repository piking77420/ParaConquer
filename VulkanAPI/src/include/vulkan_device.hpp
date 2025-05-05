#pragma once

#include "low_renderer/rhi_device.hpp"
#include "vulkan_header.h"
#include "vulkan_physical_devices.hpp"

namespace Vulkan
{
    
    class VulkanDevice : public PC_CORE::RhiDevice
    {
    public:

        vk::Device GetDevice() const;
        
        VulkanDevice(const std::shared_ptr<VulkanPhysicalDevices>& _vulkanPhysicalDevices, const std::vector<std::string>& _extensionToEnable, vk::Queue* _graphicQueue
            ,  vk::Queue* _presentQueue, vk::Queue* _transferQueue);

        VulkanDevice() = default;

        ~VulkanDevice() override;
        
    private:
        vk::Device m_Device;

    };

}
