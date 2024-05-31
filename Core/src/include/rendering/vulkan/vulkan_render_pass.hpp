#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE
    class VulkanRenderPass
    {
    public:
        void Init(const VkFormat _vkFormat);
    
        void Destroy();
    
        VkRenderPass renderPass = VK_NULL_HANDLE;
    };

END_PCCORE
