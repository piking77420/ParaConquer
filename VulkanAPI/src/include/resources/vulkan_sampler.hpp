#pragma once

#include "vulkan_header.h"
#include "low_renderer/sampler.hpp"


namespace Vulkan
{
    class VulkanSampler : public PC_CORE::Sampler
    {
    public:
        
        
        VulkanSampler(VulkanSampler&& _other) noexcept
            : Sampler(std::move(_other)), m_Sampler(_other.m_Sampler)
        {
            _other.m_Sampler = nullptr;
        }

        VulkanSampler& operator=(VulkanSampler&& _other) noexcept
        {
            if (this != &_other)
            {
                PC_CORE::Sampler::operator=(std::move(_other));

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
