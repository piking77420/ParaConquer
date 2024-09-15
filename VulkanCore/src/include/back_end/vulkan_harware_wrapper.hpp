#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    // VulkanHarwareWrapper
    class VulkanHarwareWrapper;
    using VHW = VulkanHarwareWrapper;

    class VulkanHarwareWrapper
    {
    public:
        VulkanHarwareWrapper();

        ~VulkanHarwareWrapper();

    private:
        void CreateInstance(const char* _AppName);
        
        vk::Instance m_Instance;    

        vk::PhysicalDevice m_PhysicalDevice;

        vk::Device m_Device;
    };
   
}

