#include "rendering/vulkan/vulkan_device.hpp"

#include <array>

#include "log.hpp"

using namespace PC_CORE;

void VulkanDevice::Init(const VulkanPhysicalDevices& _physicalDevices)
{
    constexpr float qGPriorities[] = { 1.f,1.f };
    constexpr float qCPriorities[] = { 1.f};

    const PhysicalDevice& physicalDevice = _physicalDevices.GetCurrentDevice();

    graphicsQueue.index = _physicalDevices.GetQueueFamilliesIndex(VK_QUEUE_GRAPHICS_BIT, true);
    computeQueue.index = _physicalDevices.GetQueueFamilliesIndex(VK_QUEUE_COMPUTE_BIT, false);
    transferQueue.index = _physicalDevices.GetQueueFamilliesIndex(VK_QUEUE_TRANSFER_BIT, false);

    VkDeviceQueueCreateInfo qGInfo =
        {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // must be zero
        .queueFamilyIndex = graphicsQueue.index,
        .queueCount = 2,
        .pQueuePriorities = &qGPriorities[0]
    };

    VkDeviceQueueCreateInfo qCInfo =
        {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // must be zero
        .queueFamilyIndex = computeQueue.index,
        .queueCount = 1,
        .pQueuePriorities = &qCPriorities[0]
    };

    VkDeviceQueueCreateInfo qTInfo =
        {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // must be zero
        .queueFamilyIndex = transferQueue.index,
        .queueCount = 1,
        .pQueuePriorities = &qCPriorities[0]
    };

    std::array<VkDeviceQueueCreateInfo,3> queusInfos =
    {
        qGInfo,
        qCInfo,
        qTInfo
    };
    
    constexpr std::array<const char*, 2> DevExts =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
    };
    
    if (physicalDevice.features.geometryShader == VK_FALSE)
        PC_LOGERROR("The Geometry Shader is not supported!")
    

    if (physicalDevice.features.tessellationShader == VK_FALSE)
        PC_LOGERROR("The Tessellation Shader is not supported!")

    
    if (physicalDevice.features.samplerAnisotropy == VK_FALSE)
        PC_LOGERROR("The Anisotropy filter is not supported!")
    
    
    
    VkPhysicalDeviceFeatures DeviceFeatures = { 0 };
    DeviceFeatures.geometryShader = VK_TRUE;
    DeviceFeatures.tessellationShader = VK_TRUE;
    DeviceFeatures.samplerAnisotropy = VK_TRUE;

    const VkDeviceCreateInfo DeviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queusInfos.size()),
        .pQueueCreateInfos = queusInfos.data(),
        .enabledLayerCount = 0,			// DEPRECATED
        .ppEnabledLayerNames = nullptr,    // DEPRECATED
        .enabledExtensionCount = static_cast<uint32_t>(DevExts.size()),
        .ppEnabledExtensionNames = DevExts.data(),
        .pEnabledFeatures = &DeviceFeatures
    };

    const VkResult res = vkCreateDevice(physicalDevice.physDevice, &DeviceCreateInfo, nullptr, &device);
    VK_CHECK_ERROR(res, "Create device");
    PC_LOG("Device created");

    vkGetDeviceQueue(device, graphicsQueue.index, 0, &graphicsQueue.Queue);
    vkGetDeviceQueue(device, computeQueue.index, 0, &computeQueue.Queue);
    vkGetDeviceQueue(device, transferQueue.index, 0, &transferQueue.Queue);

}

void VulkanDevice::Destroy()
{
    Log::Debug("Destroy Device");
    vkDestroyDevice(device, nullptr);
}
