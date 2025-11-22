#pragma once

#include <set>

#include "VulkanHeader.h"
#include "LowRenderer/PhysicalDevices.hpp"

namespace PC_CORE
{
    class RhiContext;
}

namespace Vulkan
{
    class VulkanContext;

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        vk::QueueFamilyProperties familyProperties;
        bool supportPresent;
    };

    struct VulkanPhysicalDevice : PC_CORE::PhysicalDevice
    {
        vk::PhysicalDevice physicalDevice;

        float GetMaxSamplerAnisotropy() const override;

        uint32_t GetMaxUsableSampleCount() const override;
    };


    class VulkanPhysicalDevices : public PC_CORE::PhysicalDevices
    {
    public:
        VULKAN_API vk::PhysicalDevice GetVulkanDevice() const;

        VULKAN_API const std::vector<QueueFamilyIndices>& GetQueuesFamilies();

        VULKAN_API explicit VulkanPhysicalDevices(vk::Instance _VkInstance, vk::SurfaceKHR _Surface,
                                                  const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo,
                                                  std::set<std::string>* _extensionToEnable);

        VULKAN_API ~VulkanPhysicalDevices() override;

        VULKAN_API SwapChainSupportDetails GetSwapChainSupport(const vk::SurfaceKHR& _surfaceKhr);


    private:
        vk::Instance m_VkInstance;

        std::vector<QueueFamilyIndices> m_QueuesFamiliesProperty;

        VulkanPhysicalDevice* GetSelectedPhysicalDevice();

        VULKAN_API void Initialize(const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo,
                                   std::set<std::string>* _extensionToEnable);

        VULKAN_API int32_t GetDeviceScore(const vk::PhysicalDevice& _physicalDevice,
                                          const std::set<std::string>& _requestExtensions, size_t _deviceIndex);

        VULKAN_API void LookForSuitableDevices(const std::vector<vk::PhysicalDevice>& _physicalDevices,
                                               const std::set<std::string>& _requestExtensions);

        VULKAN_API void GetDeviceProperties(PC_CORE::PhysicalDevice* _physicalDevice,
                                            const vk::PhysicalDeviceProperties& _physicalDeviceProperties,
                                            size_t* _score);

        VULKAN_API void GetDeviceFeatures(PC_CORE::PhysicalDevice* _physicalDevice,
                                          const vk::PhysicalDeviceFeatures& _physicalDeviceProperties, size_t* _score);

        VULKAN_API std::set<std::string> GetVulkanRequestExtensions(
            const std::vector<PC_CORE::RhiExtension>& _requestExtensions);

        VULKAN_API static bool CheckDeviceExtensionSupport(
            const std::vector<vk::ExtensionProperties>& availableExtensions,
            const std::set<std::string>& requiredExtensions);

        VULKAN_API void QueryQueueFamilies(vk::SurfaceKHR surface);

    };
}
