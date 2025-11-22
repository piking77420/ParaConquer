#pragma once

#include "CoreHeader.hpp"
#include "RhiTypedef.h"
#include "RhiResource.hpp"

BEGIN_PCCORE
    struct SamplerCreateInfo
    {
        std::string SamplerName;

        Filter magFilter;
        Filter minFilter;

        SamplerAddressMode u;
        SamplerAddressMode v;
        SamplerAddressMode w;
    };

    class RhiSampler : public RhiResource
    {
    public:
        Filter magFilter;
        Filter minFilter;

        SamplerAddressMode samU;
        SamplerAddressMode samV;
        SamplerAddressMode samW;

        DEFAULT_COPY_MOVE_OPERATIONS(RhiSampler)

        RhiSampler(const SamplerCreateInfo& _samplerCreateInfo);

        RhiSampler() = default;

        ~RhiSampler() override = default;
    };

END_PCCORE
