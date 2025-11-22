#include "rendering/sampler.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::Sampler::Sampler(Rhi& _rhi, const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo) 
    : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    m_RhiSampler.reset(_rhi.CreateSampler(Name, _samplerCreateInfo));
}

PC_CORE::Sampler::Sampler(Rhi& _rhi, std::string&& _name, const SamplerCreateInfo& _samplerCreateInfo)
    : Resource(std::move(_name))
{
    DYNAMIC_REFLECT_INIT
        m_RhiSampler.reset(_rhi.CreateSampler(_name, _samplerCreateInfo));
}
