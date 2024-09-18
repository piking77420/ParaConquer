#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    struct QueuFamiliesIndicies
    {
        uint32_t graphicsFamily = INVALID_QUEU;
        uint32_t presentFamily = INVALID_QUEU;
    };
    
    class VulkanPhysicalDevices
    {
    public:
        vk::PhysicalDevice GetSelectedPhysicalDevice() const;

        QueuFamiliesIndicies GetQueueFamiliesIndicies() const;

        SwapChainSupportDetails GetSwapChainSupport() const;

        void ChoosePhysicalDevice(vk::Instance _instance, vk::SurfaceKHR _SurfaceKHR ,
            std::vector<const char*> _deviceExtensions);
        
    private:
        vk::PhysicalDevice m_SelectedPhysicalDevice;

        QueuFamiliesIndicies m_QueuFamiliesIndicies;

        SwapChainSupportDetails m_SwapChainSupportDetails;

        bool IsSuitableDevice(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface , std::vector<const char*> _deviceExtensions);

        bool CheckDeviceExtensionSupport(vk::PhysicalDevice device, std::vector<const char*> _deviceExtensions);
        
        QueuFamiliesIndicies FindQueuFamillies(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface);

        static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice  _physicalDevice, vk::SurfaceKHR _surface);

#ifdef _DEBUG
        void PrintPhysicalDeviceProperties(vk::PhysicalDevice _physicalDevice);
#endif

    };
}