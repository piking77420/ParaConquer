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

    RhiFrameBuffer& SetAttachments(std::vector<RhiTexture*> _Attachements)
    {
        m_Attachments = std::move(_Attachements);
        return *this;
    }

    template<typename... Attachments>
    RhiFrameBuffer& SetAttachments(Attachments*... textures)
    {
        static_assert((std::is_same_v<Attachments, RhiTexture> && ...),
            "All attachments must be RhiTexture*");

        m_Attachments.clear();
        (m_Attachments.emplace_back(textures), ...);

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

    PC_CORE_API Tbx::Vector2ui Size() const
    {
        return {m_Width, m_Height};
    }

    PC_CORE_API const std::vector<RhiTexture*>& GetAttachements() const
    {
        return m_Attachments;
    }

    PC_CORE_API RhiRenderPass* GetRenderPass() const
    {
        return m_RenderPass;
    }


protected:
    uint32_t m_Width{ 0 };

    uint32_t m_Height{ 0 };

    std::vector<RhiTexture*> m_Attachments;

    RhiRenderPass* m_RenderPass{ nullptr };
};

END_PCCORE
