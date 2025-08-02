#pragma once

#include "low_renderer/rhi_resource.hpp"

BEGIN_PCCORE

class RhiTexture : public PC_CORE::RhiResource
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture)


    uint32_t GetMipLevelCount() const
    {
        return m_MipLevelCount;
    }

    uint32_t GetLayerCount() const
    {
        return m_LayerCount;
    }

    TextureUsage GetTextureUsage() const
    {
        return m_TextureUsage;
    }

    RhiTexture(const PC_CORE::CreateImageInfo& _createImageInfo);

    RhiTexture() = default;

    virtual ~RhiTexture() = default;

protected:
    uint32_t              m_MipLevelCount = 1;
    uint32_t              m_LayerCount = 1;
    TextureUsage m_TextureUsage = TextureUsage::All;
};

END_PCCORE