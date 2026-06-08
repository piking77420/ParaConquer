#pragma once

#include "CoreHeader.hpp"

#include "RhiRenderPass.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE

class RhiFrameBuffer : public RhiObjectT<RhiFrameBuffer>
{
public:
    PC_CORE_API RhiFrameBuffer(Rhi& _Rhi);

    PC_CORE_API ~RhiFrameBuffer() override = default;

    struct AttachementDescriptor
    {
        const RhiTexture* Texture = nullptr;
        RhiTexture::Type ViewType = {};
        uint32_t BaseLayer = 1;
        uint32_t LayerCount = 1;
        uint32_t BaseLevel = 1;
        uint32_t LevelCount = 1;
    };

    // Setter
    RhiFrameBuffer& SetWidth(uint32_t _Widht)
    {
        m_Width = _Widht;
        return *this;
    }

    RhiFrameBuffer& SetHeight(uint32_t _Height)
    {
        m_Height = _Height;
        return *this;
    }

    RhiFrameBuffer& SeLayers(uint32_t _Layer)
    {
        m_Layer = _Layer;
        return *this;
    }

    RhiFrameBuffer& ClearAttachement()
    {
        m_Attachments.clear();
        m_DepthAttachement.reset();
        return *this;
    }

    RhiFrameBuffer& SetAttachement(const RhiTexture* Texture)
    {
        return SetAttachement(Texture, Texture->GetTextureType(), 0, Texture->GetLayer(), 0, Texture->GetLevel());
    }

    RhiFrameBuffer& SetAttachement(const RhiTexture* Texture, RhiTexture::Type ViewType, uint32_t BaseLayer, uint32_t LayerCount, uint32_t BaseLevel, uint32_t LevelCount)
    {
        m_Attachments.emplace_back(Texture, ViewType, BaseLayer, LayerCount, BaseLevel, LevelCount);
        return *this;
    }

    RhiFrameBuffer& SetDepthAttachment(const RhiTexture* Texture)
    {
        return SetAttachement(Texture, Texture->GetTextureType(), 0, Texture->GetLayer(), 0, Texture->GetLevel());
    }

    RhiFrameBuffer& SetDepthAttachment(const RhiTexture* Texture, RhiTexture::Type ViewType, uint32_t BaseLayer, uint32_t LayerCount, uint32_t BaseLevel, uint32_t LevelCount)
    {
        m_DepthAttachement.emplace(Texture, ViewType, BaseLayer, LayerCount, BaseLevel, LevelCount);
        return *this;
    }

    RhiFrameBuffer& SetRenderPass(RhiRenderPass* _RhiRenderPass)
    {
        m_RenderPass = _RhiRenderPass;
        return *this;
    }

    // Getter

    PC_CORE_API uint32_t GetWidth() const
    {
        return m_Width;
    }

    PC_CORE_API uint32_t GetHeight() const
    {
        return m_Height;
    }

    PC_CORE_API uint32_t GetLayer() const
    {
        return m_Layer;
    }

    PC_CORE_API Tbx::Vector2ui Size() const
    {
        return {m_Width, m_Height};
    }

    PC_CORE_API const std::vector<AttachementDescriptor>& GetAttachments() const
    {
        return m_Attachments;
    }

    PC_CORE_API const std::optional<AttachementDescriptor>& GetDepthAttachements() const
    {
        return m_DepthAttachement;
    }

    PC_CORE_API RhiRenderPass* GetRenderPass() const
    {
        return m_RenderPass;
    }


protected:
    
    uint32_t m_Width{ 0 };

    uint32_t m_Height{ 0 };

    uint32_t m_Layer{ 1 };

    std::vector<AttachementDescriptor> m_Attachments;

    std::optional<AttachementDescriptor> m_DepthAttachement;

    RhiRenderPass* m_RenderPass{ nullptr };
};

END_PCCORE
