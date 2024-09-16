﻿#include "back_end/vulkan_physical_devices.hpp"

#include <set>

vk::PhysicalDevice VK_NP::VulkanPhysicalDevices::GetSelectedPhysicalDevice() const
{
    return m_SelectedPhysicalDevice;
}

VK_NP::QueuFamiliesIndicies VK_NP::VulkanPhysicalDevices::GetQueueFamiliesIndicies() const
{
    return m_QueuFamiliesIndicies;
}

SwapChainSupportDetails VK_NP::VulkanPhysicalDevices::GetSwapChainSupport() const
{
    return m_SwapChainSupportDetails;
}

bool VK_NP::VulkanPhysicalDevices::IsSuitableDevice(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface, std::vector<const char*> _deviceExtensions)
{
    vk::PhysicalDeviceProperties physicalDeviceProperties = _physicalDevice.getProperties();
    vk::PhysicalDeviceFeatures physicalDeviceFeatures = _physicalDevice.getFeatures();
    
    const QueuFamiliesIndicies indices = FindQueuFamillies(_physicalDevice, _surface);
    const SwapChainSupportDetails swapChainSupportDetails =  QuerySwapChainSupport(_physicalDevice, _surface);
    
    bool extensionsSupported = CheckDeviceExtensionSupport(_physicalDevice, _deviceExtensions);
    bool swapChainAdequate = !swapChainSupportDetails.formats.empty() && !swapChainSupportDetails.presentModes.empty();

    if (physicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu ||
        indices.graphicsFamily == INVALID_QUEU || indices.presentFamily == INVALID_QUEU || !extensionsSupported || !swapChainAdequate)
        return false;

    m_QueuFamiliesIndicies = indices;
    m_SwapChainSupportDetails = swapChainSupportDetails;
    
    return true;
}

bool VK_NP::VulkanPhysicalDevices::CheckDeviceExtensionSupport(vk::PhysicalDevice device,
    std::vector<const char*> _deviceExtensions)
{
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
    
    
    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VK_NP::QueuFamiliesIndicies VK_NP::VulkanPhysicalDevices::FindQueuFamillies(
    vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface)
{
    QueuFamiliesIndicies indices;

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

SwapChainSupportDetails VK_NP::VulkanPhysicalDevices::QuerySwapChainSupport(vk::PhysicalDevice _physicalDevice, vk::SurfaceKHR _surface)
{
    SwapChainSupportDetails swapChainSupportDetails;
    
    swapChainSupportDetails.capabilities = _physicalDevice.getSurfaceCapabilitiesKHR(_surface);
    swapChainSupportDetails.formats  = _physicalDevice.getSurfaceFormatsKHR(_surface);
    
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

void VK_NP::VulkanPhysicalDevices::ChoosePhysicalDevice(vk::Instance _instance, vk::SurfaceKHR _SurfaceKHR, std::vector<const char*> _deviceExtensions)
{
    std::vector<vk::PhysicalDevice> devices = _instance.enumeratePhysicalDevices();
    
    for (auto& phyDevice : devices)
    {
        if (IsSuitableDevice(phyDevice, _SurfaceKHR, _deviceExtensions))
        {
            m_SelectedPhysicalDevice = phyDevice;
            break;
        }
    }

    if (m_SelectedPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}