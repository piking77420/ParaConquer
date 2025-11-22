#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiSampler.hpp"


namespace Vulkan
{
    class VulkanSampler : public PC_CORE::RhiSampler
    {
    public:
        bool Build() override;

        const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &m_Sampler;
        }

        void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &m_Sampler;
        }
        
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanSampler)

        explicit VulkanSampler(PC_CORE::Rhi& _Rhi, const std::string& _name, const PC_CORE::SamplerCreateInfo& _samplerCreateInfo);

        ~VulkanSampler() override;

        vk::Sampler GetSampler() const;

    private:
        vk::Sampler m_Sampler;
    };
}
