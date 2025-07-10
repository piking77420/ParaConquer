#include "low_renderer/rhi_sampler.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::RhiSampler::RhiSampler(const SamplerCreateInfo& _samplerCreateInfo) : magFilter(_samplerCreateInfo.magFilter)
                                                                         , minFilter(_samplerCreateInfo.minFilter), samU(_samplerCreateInfo.u) , samV(_samplerCreateInfo.v) , samW(_samplerCreateInfo.w)
{
    
}
