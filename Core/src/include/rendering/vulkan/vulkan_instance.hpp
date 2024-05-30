#pragma once

#include "core_header.hpp"
#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanInstance
{
public:
    VkInstance instance = VK_NULL_HANDLE;
    
    void Init();

    void Destroy();
    
private:
};

END_PCCORE