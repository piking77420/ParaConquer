#pragma once

#include "GpuResource.hpp"
#include "LowRenderer/RhiSampler.hpp"
#include "Resources/Resource.hpp"

BEGIN_PCCORE
class PC_CORE_API Sampler : public Resource, public IGpuResource
{
public:
    std::shared_ptr<RhiResource> GetRhiHandle() const override
    {
        return m_Sampler;
    }

    std::shared_ptr<RhiSampler> GetRhiSampler() const
    {
        return m_Sampler;
    }

    IMP_DYNAMIC_REFLECT()
    
    explicit Sampler(const SamplerCreateInfo& _samplerCreateInfo);
    
    explicit Sampler()
    {
        DYNAMIC_REFLECT_INIT;
    }
    
    ~Sampler() override = default;
    
private:
    std::shared_ptr<RhiSampler> m_Sampler;

    REFLECT(Sampler);
    
};

END_PCCORE