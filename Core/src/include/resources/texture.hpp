#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "rendering/vulkan/vulkan_texture.hpp"

BEGIN_PCCORE
    class Texture : public IResource
{
public:
    Vector2i textureSize;
    
    int textureChannel = -1;
    
    VulkanTexture vulkanTexture;
    
    ~Texture() override;
    
    void Load(const fs::path& path) override;
    
};

END_PCCORE