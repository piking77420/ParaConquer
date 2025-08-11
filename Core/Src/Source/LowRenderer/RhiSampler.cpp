#include "LowRenderer/RhiSampler.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiSampler::RhiSampler(const SamplerCreateInfo& _samplerCreateInfo) : magFilter(_samplerCreateInfo.magFilter)
                                                                         , minFilter(_samplerCreateInfo.minFilter), samU(_samplerCreateInfo.u) , samV(_samplerCreateInfo.v) , samW(_samplerCreateInfo.w)
{
    
}
