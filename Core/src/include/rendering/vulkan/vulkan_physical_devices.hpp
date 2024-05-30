#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanPhysicalDevices
{
public:
    void Init(VkInstance _instance,VkSurfaceKHR _surfaceKhr);
    
    uint32_t SelectDevice(VkQueueFlags _RequiredQueueType, bool _SupportsPresent);

    const PhysicalDevice& GetCurrentDevice() const;

private:
    std::vector<PhysicalDevice> m_Devices;

    int32_t m_CurrentDevice = -1;
};

END_PCCORE