#pragma once

#include "vulkan_header.h"
#include "low_renderer/render_instance.hpp"

namespace Vulkan
{
    class VulkanInstance : public PC_CORE::RenderInstance
    {
    public:
        VULKAN_API explicit VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo);
        
        VULKAN_API explicit VulkanInstance() = default;

        VULKAN_API ~VulkanInstance() override;
        
        VULKAN_API inline vk::Instance Get() const
        {
            return m_Instance;
        }

    private:
        vk::Instance m_Instance;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        
#ifdef _DEBUG

        bool CheckValidationLayerSupport();

        void SetupDebugMessenger();

        void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _debugMessengerCreateInfo);
        #endif
    };
}
