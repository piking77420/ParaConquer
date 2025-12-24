#include "rendering/sampler.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::Sampler::Sampler(Rhi& _rhi, const std::string& _name) 
    : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    m_RhiSampler.reset(_rhi.CreateSampler());
    m_RhiSampler->SetName(_name);
}

PC_CORE::Sampler::Sampler(Rhi& _rhi, std::string&& _name)
    : Resource(std::move(_name))
{
    DYNAMIC_REFLECT_INIT
    m_RhiSampler.reset(_rhi.CreateSampler());
    m_RhiSampler->SetName(std::move(_name));
}
