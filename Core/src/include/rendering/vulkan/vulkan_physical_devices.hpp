﻿#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanPhysicalDevices
{
public:
    void Init(VkInstance _instance,VkSurfaceKHR _surfaceKhr);
    
    // function that select the corresponding device
    void SelectDevice(VkQueueFlags _RequiredQueueType, bool _SupportsPresent);

    uint32_t GetQueueFamilliesIndex(VkQueueFlags _RequiredQueueType, bool _SupportsPresent) const;
    
    const PhysicalDevice& GetCurrentDevice() const;

    PhysicalDevice& GetCurrentDevice();

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features) const;

    VkFormat FindDepthFormat() const;

    bool HasStencilComponent(VkFormat format);
    
private:
    std::vector<PhysicalDevice> m_Devices;

    int32_t m_CurrentDevice = -1;

    void ComputeQueue(const PhysicalDevice& queue);
};

END_PCCORE