#pragma once

#include "vulkan_header.h"
#include "vulkan_physical_devices.hpp"

BEGIN_PCCORE
    class VulkanDevice
{
public:
    struct QueuAndIndex
    {
        uint32_t index = -1;
        VkQueue Queue = VK_NULL_HANDLE;
    };
    
    VkDevice device = VK_NULL_HANDLE;

    QueuAndIndex graphicsQueue;

    QueuAndIndex computeQueue;

    
    void Init(const VulkanPhysicalDevices& _physicalDevice);

    void Destroy();

private:
};

END_PCCORE