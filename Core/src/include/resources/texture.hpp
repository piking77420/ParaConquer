#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE



    struct CreateTextureInfo
    {
        int32_t width;
        int32_t height;
        RHIFormat format;
        ImageAspectFlagBits imageAspectFlagBits;
    };

class Texture : public Resource
{
public:
    
    int textureChannel = -1;

    PC_CORE_API Texture() = default;

    PC_CORE_API Texture(const CreateTextureInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    
    PC_CORE_API void Load(std::array<std::string,6>& _maps);

    PC_CORE_API void CreateTexture();

    PC_CORE::ImageHandle GetImageHandle();

    PC_CORE::ImageHandle GetImageViewHandle();

    PC_CORE::ImageHandle GetSamplerHandle();

    Tbx::Vector2i GetTextureSize() const;

private:
    PC_CORE::ImageHandle m_ImageHandle = nullptr;

    PC_CORE::ImageViewHandle m_ImageViewHandle = nullptr;

    PC_CORE::SamplerHandle m_SamplerHandle = nullptr;

    uint32_t m_MipLevel = 0;

    Tbx::Vector2i m_TextureSize;

    void CreateTextureFromFile(const fs::path& _path);
};


END_PCCORE