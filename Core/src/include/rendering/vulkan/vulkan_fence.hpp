#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanFence
{
public:

    VkFence fences = VK_NULL_HANDLE;
    
    void Init(const VkFenceCreateInfo& _semaphoreCreateInfo);
    
    void Destroy();
};

END_PCCORE