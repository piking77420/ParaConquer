#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDevice
{
public:
    VkDevice device = VK_NULL_HANDLE;

    void Init(const PhysicalDevice& _physicaldevice, uint32_t queuFamily);

    void Destroy();

private:
};

END_PCCORE