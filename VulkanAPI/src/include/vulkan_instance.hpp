#pragma once

#include "vulkan_header.h"
#include "low_renderer/render_instance.hpp"

namespace Vulkan
{
    class VulkanInstance : public PC_CORE::RenderInstance
    {
    public:

        vk::SurfaceKHR surface;

        VULKAN_API explicit VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo, GLFWwindow* _window);
        
        VULKAN_API explicit VulkanInstance() = default;

        VULKAN_API ~VulkanInstance() override;
        
        VULKAN_API inline vk::Instance Get() const
        {
            return m_Instance;
        }

        inline vk::Instance GetVulkanInstance() const
        {
            return m_Instance;
        }

#ifdef PROFILING
        PFN_vkCmdBeginDebugUtilsLabelEXT GetPFN_vkCmdBeginDebugUtilsLabelEXT() const
        {
            return m_BeginDebugLabel;
        }

        PFN_vkCmdEndDebugUtilsLabelEXT GetPFN_vkCmdEndDebugUtilsLabelEXT() const
        {
            return m_EndDebugLabel;
        }
#endif

    private:
        vk::Instance m_Instance;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        void InitSurface(GLFWwindow* _window);

#ifdef _DEBUG

        bool CheckValidationLayerSupport();

        void SetupDebugMessenger();

        void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _debugMessengerCreateInfo);
#endif

#ifdef PROFILING
        PFN_vkCmdBeginDebugUtilsLabelEXT m_BeginDebugLabel;

        PFN_vkCmdEndDebugUtilsLabelEXT m_EndDebugLabel;

        void GetDebugFunc();
#endif // PROFILING

    };
}
