#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "LowRenderer/RhiTexture.hpp"
#include "Rendering/GpuResource.hpp"

BEGIN_PCCORE
    class Texture : public Resource, public IGpuResource
    {
    public:    
        

        explicit Texture(const std::string& _name) : Resource(_name)
        {
            
        }

        explicit Texture(std::unique_ptr<PC_CORE::RhiTexture> _Texture);

        PC_CORE_API explicit Texture()
        {
            DYNAMIC_REFLECT_INIT;
        }
    
        ~Texture() override = default;

        Texture(Texture&&) noexcept = default;
        
        Texture& operator=(Texture&&) noexcept = default;

        IMP_DYNAMIC_REFLECT()
    
        RhiTexture* const Get() const
        {
            return m_RhiTexture.get();
        }
        
        RhiTexture* const operator->() const
        {
            return m_RhiTexture.get();
        }
    
    protected:
        REFLECT(Texture, Resource)
        
        std::unique_ptr<RhiTexture> m_RhiTexture;
    };


END_PCCORE
