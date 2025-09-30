#pragma once

#include "LowRenderer/RhiDevice.hpp"
#include "VulkanHeader.h"
#include "VulkanPhysicalDevices.hpp"

namespace Vulkan
{
    
    class VulkanDevice : public PC_CORE::RhiDevice
    {
    public:

        vk::Device GetDevice() const;
        
        VulkanDevice(const std::shared_ptr<VulkanPhysicalDevices>& _vulkanPhysicalDevices, const std::set<std::string>& _extensionToEnable, vk::Queue* _graphicQueue);

        VulkanDevice() = default;

        ~VulkanDevice() override;

#ifdef  PROFILING
        PFN_vkResetQueryPoolEXT GetPFN_vkResetQueryPoolEXT() const
        {
            return m_Qpreset;
        }

        PFN_vkGetCalibratedTimestampsEXT GetPFN_vkGetCalibratedTimestampsEXT() const
        {
            return m_Gct;
        }
#endif

        
    private:
        vk::Device m_Device;

        void GetExtensionFunctions();

#ifdef PROFILING
        PFN_vkResetQueryPoolEXT m_Qpreset = nullptr;

        PFN_vkGetCalibratedTimestampsEXT m_Gct = nullptr;
#endif 

    };

}
