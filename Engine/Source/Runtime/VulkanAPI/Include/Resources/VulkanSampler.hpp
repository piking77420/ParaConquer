#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiSampler.hpp"


namespace Vulkan
{
    class VulkanSampler : public PC_CORE::RhiSampler
    {
    public:
        const void* GetNativeHandle() const override
        {
            return &m_Sampler;
        }
        
        void* GetNativeHandle() override
        {
            return &m_Sampler;
        }
        
        VulkanSampler(VulkanSampler&& _other) noexcept
            : RhiSampler(std::move(_other)), m_Sampler(_other.m_Sampler)
        {
            _other.m_Sampler = nullptr;
        }

        VulkanSampler& operator=(VulkanSampler&& _other) noexcept
        {
            if (this != &_other)
            {
                PC_CORE::RhiSampler::operator=(std::move(_other));

                m_Sampler = _other.m_Sampler;
                _other.m_Sampler = nullptr;
            }
            return *this;
        }

        VulkanSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo);

        VulkanSampler() = default;

        ~VulkanSampler() override;

        vk::Sampler GetSampler() const;

    private:
        vk::Sampler m_Sampler;
    };
}
