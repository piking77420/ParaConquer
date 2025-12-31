#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiSampler.hpp"


namespace Vulkan
{
    class VulkanSampler : public PC_CORE::RhiSampler
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanSampler)

        VULKAN_API explicit VulkanSampler(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanSampler() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &m_Sampler;
        }

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &m_Sampler;
        }
        
        VULKAN_API bool Build() override;
        
        vk::Sampler GetVkSampler() const;

    private:
        vk::Sampler m_Sampler;
    };
}
