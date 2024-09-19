﻿#include "back_end/vulkan_physical_devices.hpp"
#include <vulkan/vk_enum_string_helper.h>


#include <set>


bool VK_NP::VulkanPhysicalDevices::IsSuitableDevice(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface,
                                                    std::vector<const char*> _deviceExtensions, QueuFamiliesIndicies* _outQueuFamiliesIndicies,
    SwapChainSupportDetails* _swapChainSupportDetails)
{
    vk::PhysicalDeviceProperties physicalDeviceProperties = _physicalDevice.getProperties();
    vk::PhysicalDeviceFeatures physicalDeviceFeatures = _physicalDevice.getFeatures();

    const QueuFamiliesIndicies indices = FindQueuFamillies(_physicalDevice, _surface);
    const SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(_physicalDevice, _surface);

    bool extensionsSupported = CheckDeviceExtensionSupport(_physicalDevice, _deviceExtensions);
    bool swapChainAdequate = !swapChainSupportDetails.formats.empty() && !swapChainSupportDetails.presentModes.empty();

    if (physicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu ||
        indices.graphicsFamily == INVALID_QUEU || indices.presentFamily == INVALID_QUEU || !extensionsSupported || !
        swapChainAdequate)
        return false;

    *_outQueuFamiliesIndicies = indices;
    *_swapChainSupportDetails = swapChainSupportDetails;

    return true;
}

bool VK_NP::VulkanPhysicalDevices::CheckDeviceExtensionSupport(const vk::PhysicalDevice device,
                                                               std::vector<const char*> _deviceExtensions)
{
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();


    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VK_NP::QueuFamiliesIndicies VK_NP::VulkanPhysicalDevices::FindQueuFamillies(
    const vk::PhysicalDevice _physicalDevice, const vk::SurfaceKHR _surface)
{
    QueuFamiliesIndicies indices = { INVALID_QUEU, INVALID_QUEU }; // Initialize to invalid indices

    std::vector<vk::QueueFamilyProperties> queueFamilies = _physicalDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        vk::Bool32 presentSupport = false;
        VK_CALL(static_cast<vk::Result>(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &presentSupport)));
        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        i++;
    }

    return indices;
}

VK_NP::SwapChainSupportDetails VK_NP::VulkanPhysicalDevices::QuerySwapChainSupport(
    const vk::PhysicalDevice _physicalDevice, const vk::SurfaceKHR _surface)
{
    SwapChainSupportDetails swapChainSupportDetails;

    swapChainSupportDetails.capabilities = _physicalDevice.getSurfaceCapabilitiesKHR(_surface);
    swapChainSupportDetails.formats = _physicalDevice.getSurfaceFormatsKHR(_surface);

    if (swapChainSupportDetails.formats.empty())
    {
        throw std::runtime_error("NONE Surface format not available");
    }
    swapChainSupportDetails.presentModes = _physicalDevice.getSurfacePresentModesKHR(_surface);

    if (swapChainSupportDetails.presentModes.empty())
    {
        throw std::runtime_error("NONE PresentMode not available");
    }

    return swapChainSupportDetails;
}
#ifdef _DEBUG
void VK_NP::VulkanPhysicalDevices::PrintPhysicalDeviceProperties(vk::PhysicalDevice _physicalDevice)
{
    vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();

    std::cout << "Selected Device Properties: " << '\n';
    std::cout << "Device Name: " << properties.deviceName << std::endl;
    std::cout << "Device Type: " << string_VkPhysicalDeviceType(
            static_cast<VkPhysicalDeviceType>(properties.deviceType)) <<
        '\n';
}
#endif
vk::PhysicalDevice VK_NP::VulkanPhysicalDevices::ChoosePhysicalDevice(VulkanContext* _vulkanContext,
                                                        std::vector<const char*> _deviceExtensions)
{
    std::vector<vk::PhysicalDevice> devices = _vulkanContext->instance.enumeratePhysicalDevices();
    vk::PhysicalDevice returnPhysicalDevice = VK_NULL_HANDLE;

    for (auto& phyDevice : devices)
    {
        if (IsSuitableDevice(phyDevice, _vulkanContext->surface, _deviceExtensions,
            &_vulkanContext->queuFamiliesIndicies, &_vulkanContext->swapChainSupportDetails))
        {
            returnPhysicalDevice = phyDevice;
            break;
        }
    }

    if (returnPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

#ifdef _DEBUG
    PrintPhysicalDeviceProperties(returnPhysicalDevice);
#endif

    return returnPhysicalDevice;
}
