#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
   
    class VulkanPhysicalDevices
    {
    public:
        [[nodiscard]]
        vk::PhysicalDevice ChoosePhysicalDevice(VulkanContext* _vulkanContext, 
            std::vector<const char*> _deviceExtensions);

        QueuFamiliesIndicies FindQueuFamillies(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface);

        static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice  _physicalDevice, vk::SurfaceKHR _surface);

    private:
        bool IsSuitableDevice(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface,
            std::vector<const char*> _deviceExtensions, QueuFamiliesIndicies* _outQueuFamiliesIndicies, SwapChainSupportDetails* _swapChainSupportDetails);

        bool CheckDeviceExtensionSupport(vk::PhysicalDevice device, std::vector<const char*> _deviceExtensions);

        bool HasAllNeededFeatures(const vk::PhysicalDeviceFeatures& physicalDeviceFeatures);

#ifdef _DEBUG
        void PrintPhysicalDeviceProperties(vk::PhysicalDevice _physicalDevice);
#endif

    };
}