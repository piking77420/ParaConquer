#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE
    class Texture : public Resource
{
public:
    Tbx::Vector2i textureSize;
    
    int textureChannel = -1;

    Texture();
    
    Texture(const fs::path& _path);

    ~Texture() override;
    
    PC_CORE_API void Load(std::array<std::string,6>& _maps);

    PC_CORE::ImageHandle GetImageHandle();

    PC_CORE::ImageHandle GetImageViewHandle();

    PC_CORE::ImageHandle GetSamplerHandle();


private:
    PC_CORE::ImageHandle m_ImageHandle;

    PC_CORE::ImageViewHandle m_ImageViewHandle;

    PC_CORE::SamplerHandle m_SamplerHandle;

    uint32_t m_MipLevel = 0;

    void CreateTextureFromFile(const fs::path& _path);
};


END_PCCORE