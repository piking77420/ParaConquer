#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "LowRenderer/RhiTexture.hpp"
#include "Rendering/GpuResource.hpp"

BEGIN_PCCORE
    class Texture : public Resource, public IGpuResource
    {
    public:
    
        DEFAULT_COPY_MOVE_OPERATIONS(Texture)
    
        IMP_DYNAMIC_REFLECT()

        PC_CORE_API explicit Texture()
        {
            DYNAMIC_REFLECT_INIT;
        }

        explicit Texture(const std::string& _name) : Resource(_name)
        {
            
        }

        explicit Texture(std::unique_ptr<PC_CORE::RhiTexture> _Texture);
    
        ~Texture() override = default;
    
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
