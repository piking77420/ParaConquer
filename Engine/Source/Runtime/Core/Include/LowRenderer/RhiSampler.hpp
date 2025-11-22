#pragma once

#include "RhiObject.hpp"

BEGIN_PCCORE
    struct SamplerCreateInfo
    {
        Filter magFilter;
        Filter minFilter;

        SamplerAddressMode u;
        SamplerAddressMode v;
        SamplerAddressMode w;
    };

    class RhiSampler : public RhiObject
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(RhiSampler)

        explicit RhiSampler(Rhi& _Rhi, const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo);

        ~RhiSampler() override = default;
        
    protected:
        SamplerCreateInfo m_SamplerCreateInfo{};
    };

END_PCCORE
