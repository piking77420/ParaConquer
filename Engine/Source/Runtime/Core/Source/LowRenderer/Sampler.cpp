#include "rendering/sampler.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::Sampler::Sampler(const SamplerCreateInfo& _samplerCreateInfo) : Resource(_samplerCreateInfo.SamplerName)
{
    DYNAMIC_REFLECT_INIT
    m_Sampler = Rhi::CreateSampler(_samplerCreateInfo);
}
