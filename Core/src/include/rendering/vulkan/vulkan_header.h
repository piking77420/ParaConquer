﻿#pragma once

#include <array>
#include <vector>

#include "core_header.hpp"
#include <vulkan/vulkan.h>
#include <stdexcept>

#define VK_CHECK_ERROR(x,message)\
    if (x != VK_SUCCESS) \
    {\
        Log::Error(message,__FILE__,std::to_string(__LINE__).c_str());\
        throw std::runtime_error(message);\
    }\

struct PhysicalDevice {
    VkPhysicalDevice physDevice;
    VkPhysicalDeviceProperties devProps;
    std::vector<VkQueueFamilyProperties> qFamilyProps;
    mutable std::vector<bool> qFamilyPropsStatus; 
    std::vector<VkBool32> qSupportsPresent;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    VkSurfaceCapabilitiesKHR surfaceCaps;
    VkPhysicalDeviceMemoryProperties memProps;
    std::vector<VkPresentModeKHR> presentModes;
    VkPhysicalDeviceFeatures features;
};
