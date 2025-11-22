#pragma once

#include "GpuResource.hpp"
#include "LowRenderer/RhiSampler.hpp"
#include "Resources/Resource.hpp"

BEGIN_PCCORE
    class PC_CORE_API Sampler : public Resource, public IGpuResource
    {
    public:
    
        DEFAULT_COPY_MOVE_OPERATIONS(Sampler);

        IMP_DYNAMIC_REFLECT()

        explicit Sampler(Rhi& _rhi, const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo);

        explicit Sampler(Rhi& _rhi, std::string&& _name, const SamplerCreateInfo& _samplerCreateInfo);

        explicit Sampler()
        {
            DYNAMIC_REFLECT_INIT;
        }

        ~Sampler() override = default;
    
        RhiSampler* const Get() const
        {
            return m_RhiSampler.get();
        }
            
        RhiSampler* const operator->() const
        {
            return m_RhiSampler.get();
        }

    private:
        REFLECT(Sampler);
    
        std::unique_ptr<RhiSampler> m_RhiSampler;
    };

END_PCCORE
