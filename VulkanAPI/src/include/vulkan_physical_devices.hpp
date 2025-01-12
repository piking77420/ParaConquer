#pragma once

#include "vulkan_header.h"
#include "low_renderer/physical_devices.hpp"

namespace Vulkan
{
    class VulkanPhysicalDevices : public PC_CORE::PhysicalDevices
    {
    public:
        VULKAN_API explicit VulkanPhysicalDevices(const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo);

        VULKAN_API VulkanPhysicalDevices() = default;

        VULKAN_API virtual ~VulkanPhysicalDevices() = default;
    
    private:
        vk::PhysicalDevice m_CurrentVulkanDevices = VK_NULL_HANDLE;

        VULKAN_API int32_t GetDeviceScore(const vk::PhysicalDevice& _physicalDevice, size_t _deviceIndex);

        VULKAN_API void LookForSuitableDevices(const std::vector<vk::PhysicalDevice>& _physicalDevices, std::vector<std::string> _requestExtensions);

        VULKAN_API void GetDeviceProperties(PC_CORE::PhysicalDevice* _physicalDevice,
                                            const vk::PhysicalDeviceProperties& _physicalDeviceProperties);

        VULKAN_API void GetDeviceFeatures(PC_CORE::PhysicalDevice* _physicalDevice,
                                          const vk::PhysicalDeviceFeatures& _physicalDeviceProperties);


        VULKAN_API std::vector<std::string> GetVulkanRequestExtensions(const std::vector<std::string>& _requestExtensions);
    };
}
