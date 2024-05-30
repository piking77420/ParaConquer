#pragma once

#include "vulkan_header.h" 
#include "window.hpp"

BEGIN_PCCORE
    class VulkanSurface
{
public:
    VkSurfaceKHR surfaceKhr = VK_NULL_HANDLE;

    void Init(const VkInstance& instance, GLFWwindow* window);

    void Destroy(const VkInstance& instance);
    
private:
};

END_PCCORE