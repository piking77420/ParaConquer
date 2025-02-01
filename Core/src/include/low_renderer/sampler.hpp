#pragma once

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE
    struct SamplerCreateInfo
    {
        Filter magFilter;
        Filter minFilter;

        SamplerAddressMode u;
        SamplerAddressMode v;
        SamplerAddressMode w;
    };

class Sampler 
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(Sampler)

    Sampler(const SamplerCreateInfo& _samplerCreateInfo);
    
    Sampler() = default;
    
    virtual ~Sampler() = default;

protected:
    Filter m_MagFilter;
    Filter m_MinFilter;

    SamplerAddressMode m_U;
    SamplerAddressMode m_V;
    SamplerAddressMode m_W;
    
};

END_PCCORE