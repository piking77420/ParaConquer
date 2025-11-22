#include "LowRenderer/RhiSampler.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiSampler::RhiSampler(Rhi& _Rhi, const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo)
    : RhiObject(_Rhi, _name)
    , m_SamplerCreateInfo(_samplerCreateInfo)
{
}
