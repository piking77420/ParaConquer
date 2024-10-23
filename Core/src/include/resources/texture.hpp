#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
class Texture : public Resource
{
public:
    Tbx::Vector2i textureSize;
    
    int textureChannel = -1;
    
    //VulkanTexture vulkanTexture;
    
    ~Texture() override;
    
    PC_CORE_API void SetPath(const fs::path& path) override;

    PC_CORE_API void Load(std::array<std::string,6>& _maps);
};


END_PCCORE