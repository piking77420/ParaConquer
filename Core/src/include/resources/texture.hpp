#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "low_renderer/rhi_typedef.h"


BEGIN_PCCORE


class Texture : public Resource
{
public:
    
    PC_CORE_API Texture(Texture&& _other) noexcept 
    {
        
    }

    PC_CORE_API Texture& operator=(Texture&& _other) noexcept
    {

        return *this;
    }
    
    PC_CORE_API Texture() = default;

    PC_CORE_API Texture(const CreateTextureInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    

    PC_CORE_API void Load(const std::array<std::string,6>& _maps);
   

private:
    int m_TextureChannel = -1;

    void CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo);
};


END_PCCORE