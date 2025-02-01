#include "low_renderer/sampler.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::Sampler::Sampler(const SamplerCreateInfo& _samplerCreateInfo) : m_MagFilter(_samplerCreateInfo.magFilter)
                                                                         , m_MinFilter(_samplerCreateInfo.minFilter), m_U(_samplerCreateInfo.u) , m_V(_samplerCreateInfo.v) , m_W(_samplerCreateInfo.w)
{
    
}
