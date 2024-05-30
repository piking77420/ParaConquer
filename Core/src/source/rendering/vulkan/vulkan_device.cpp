#include "rendering/vulkan/vulkan_device.hpp"

#include "log.hpp"

using namespace PC_CORE;

void VulkanDevice::Init(const PhysicalDevice& _physicaldevice, uint32_t queuFamily)
{
    float qPriorities[] = { 1.0f };

    const PhysicalDevice& selectPhysicalDevice = _physicaldevice;

    VkDeviceQueueCreateInfo qInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // must be zero
        .queueFamilyIndex = queuFamily,
        .queueCount = 1,
        .pQueuePriorities = &qPriorities[0]
    };

    std::vector<const char*> DevExts =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
    };


    if (selectPhysicalDevice.features.geometryShader == VK_FALSE)
        PC_LOGERROR("The Geometry Shader is not supported!");
    

    if (selectPhysicalDevice.features.tessellationShader == VK_FALSE)
        PC_LOGERROR("The Tessellation Shader is not supported!");
    
    
    VkPhysicalDeviceFeatures DeviceFeatures = { 0 };
    DeviceFeatures.geometryShader = VK_TRUE;
    DeviceFeatures.tessellationShader = VK_TRUE;

    const VkDeviceCreateInfo DeviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &qInfo,
        .enabledLayerCount = 0,			// DEPRECATED
        .ppEnabledLayerNames = nullptr,    // DEPRECATED
        .enabledExtensionCount = static_cast<uint32_t>(DevExts.size()),
        .ppEnabledExtensionNames = DevExts.data(),
        .pEnabledFeatures = &DeviceFeatures
    };

    const VkResult res = vkCreateDevice(selectPhysicalDevice.physDevice, &DeviceCreateInfo, nullptr, &device);
    VK_CHECK_ERROR(res, "Create device");

    PC_LOG("Device created");
}

void VulkanDevice::Destroy()
{
    Log::Debug("Destroy Device");
    vkDestroyDevice(device, nullptr);
}
