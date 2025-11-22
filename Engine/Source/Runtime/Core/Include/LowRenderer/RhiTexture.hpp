#pragma once

#include "RhiTypedef.h"
#include "LowRenderer/RhiResource.hpp"

BEGIN_PCCORE
    class RhiTexture : public RhiResource
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

        RhiFormat GetFormat() const
        {
            return m_RhiFormat;
        }

        RhiTexture(const CreateImageInfo& _createImageInfo);

        RhiTexture() = default;

        ~RhiTexture() override = default;

    protected:
        uint32_t m_MipLevelCount = 1;

        uint32_t m_LayerCount = 1;

        TextureUsage m_TextureUsage = TextureUsage::All;

        RhiFormat m_RhiFormat;
    };

END_PCCORE
