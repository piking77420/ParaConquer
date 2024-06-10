#pragma once

#include "vulkan_header.h"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
class VulkanViewport
{
public:
    Vector2i viewPortSize = {800,600};
    
    VkImage image = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;

    VulkanViewport();

    ~VulkanViewport();
};

END_PCCORE