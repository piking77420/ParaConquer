#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RenderInstance.hpp"

namespace Vulkan
{
    class VulkanInstance : public PC_CORE::RenderInstance
    {
    public:
        vk::SurfaceKHR surface;

        VULKAN_API explicit VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo,
                                           GLFWwindow* _window);

        VULKAN_API explicit VulkanInstance() = default;

        VULKAN_API ~VulkanInstance() override;

        VULKAN_API vk::Instance Get() const
        {
            return m_Instance;
        }

        vk::Instance GetVulkanInstance() const
        {
            return m_Instance;
        }

#ifdef DEBUG_GPU_ON
        PFN_vkCmdBeginDebugUtilsLabelEXT GetPFN_vkCmdBeginDebugUtilsLabelEXT() const
        {
            return m_BeginDebugLabel;
        }

        PFN_vkCmdEndDebugUtilsLabelEXT GetPFN_vkCmdEndDebugUtilsLabelEXT() const
        {
            return m_EndDebugLabel;
        }

        void SetDebugName(vk::Device _device, const vk::DebugUtilsObjectNameInfoEXT* pNameInfo);
#endif

#ifdef  PROFILING
        PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT GetPFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT() const
        {
            return m_Gpdctd;
        }
#endif

    private:
        vk::Instance m_Instance;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        void InitSurface(GLFWwindow* _window);

        void GetDebugFunc();

#ifdef DEBUG_GPU_ON

        bool CheckValidationLayerSupport();

        void SetupDebugMessenger();

        void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _debugMessengerCreateInfo);

        PFN_vkCmdBeginDebugUtilsLabelEXT m_BeginDebugLabel;

        PFN_vkCmdEndDebugUtilsLabelEXT m_EndDebugLabel;

        PFN_vkSetDebugUtilsObjectNameEXT m_DebugName;

        PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT m_Gpdctd = nullptr;

#endif  DEBUG_GPU_ON
    };
}
