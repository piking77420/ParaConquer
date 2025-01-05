#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"


BEGIN_PCCORE


class Texture : public Resource
{
public:
    
    int textureChannel = -1;

    PC_CORE_API Texture(Texture&& _other) noexcept 
    {
        Destroy();

        m_ImageHandle = _other.m_ImageHandle;
        _other.m_ImageHandle = nullptr;

        m_ImageViewHandle = _other.m_ImageViewHandle;
        _other.m_ImageViewHandle = nullptr;

        m_SamplerHandle = _other.m_SamplerHandle;
        _other.m_SamplerHandle = nullptr;

        textureChannel = _other.textureChannel;
        _other.textureChannel = 0;

        m_MipLevel = _other.m_MipLevel;
        _other.m_MipLevel = 0;

        m_TextureSize = _other.m_TextureSize;
        _other.m_TextureSize = {};
    }

    PC_CORE_API Texture& operator=(Texture&& _other) noexcept
    {
        Destroy();

        m_ImageHandle = _other.m_ImageHandle;
        _other.m_ImageHandle = nullptr;

        m_ImageViewHandle = _other.m_ImageViewHandle;
        _other.m_ImageViewHandle = nullptr;

        m_SamplerHandle = _other.m_SamplerHandle;
        _other.m_SamplerHandle = nullptr;

        textureChannel = _other.textureChannel;
        _other.textureChannel = 0;

        m_MipLevel = _other.m_MipLevel;
        _other.m_MipLevel = 0;

        m_TextureSize = _other.m_TextureSize;
        _other.m_TextureSize = {};
    
    
        return *this;
    }
    
    PC_CORE_API Texture() = default;

    PC_CORE_API Texture(const CreateTextureInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    
    PC_CORE_API void Load(const std::array<std::string,6>& _maps);
    
    PC_CORE_API PC_CORE::ImageHandle GetImageHandle() const;

    PC_CORE_API PC_CORE::ImageHandle GetImageViewHandle() const; 

    PC_CORE_API PC_CORE::ImageHandle GetSamplerHandle() const;

    PC_CORE_API Tbx::Vector2i GetTextureSize() const;

    PC_CORE_API DescriptorImageInfo GetDescriptorImageInfo() const;

private:
    PC_CORE::ImageHandle m_ImageHandle = nullptr;

    PC_CORE::ImageViewHandle m_ImageViewHandle = nullptr;

    PC_CORE::SamplerHandle m_SamplerHandle = nullptr;

    uint32_t m_MipLevel = 0;

    Tbx::Vector2i m_TextureSize;

    uint32_t m_Depth = 0;

    RHIFormat m_Format;
    
    void Destroy();

    void CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo);
    
    void LoadCubeMap(const std::array<std::string,6>& _maps);


    void CreateMimmap();
    
    void CreateSampler();
};


END_PCCORE