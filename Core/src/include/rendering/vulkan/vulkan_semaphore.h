#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "core_header.hpp"

BEGIN_PCCORE

class VulkanSemaphore
{
public:
    VkSemaphore semaphore = VK_NULL_HANDLE;

    
    void Init(const VkSemaphoreCreateInfo& semaphoreCreateInfo);
    
    void Destroy();
private:
};

END_PCCORE