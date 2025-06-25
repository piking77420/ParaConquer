#include "rendering/sampler.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::Sampler::Sampler(const SamplerCreateInfo& _samplerCreateInfo) : Resource(_samplerCreateInfo.SamplerName)
{
    DYNAMIC_REFLECT_INIT
    m_Sampler = Rhi::CreateSampler(_samplerCreateInfo);
}
