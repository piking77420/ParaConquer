#pragma once

#include "CoreHeader.hpp"

#include "RhiRenderPass.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE
    struct FrameBufferAttachementDesriptor
    {
        RhiTexture* RhiTexture;
    };

    struct CreateFrameInfo
    {
        uint32_t Width;
        uint32_t Height;

        std::vector<FrameBufferAttachementDesriptor>* Attachements;
        RhiRenderPass* RenderPass;
    };

    class RhiFrameBuffer : public RhiObjectT<RhiFrameBuffer>
    {
    public:
        PC_CORE_API RhiFrameBuffer(Rhi& _Rhi, const std::string& _name, uint32_t _width, uint32_t m_height);

        PC_CORE_API RhiFrameBuffer(Rhi& _Rhi, std::string&& _name, uint32_t _width, uint32_t m_height);

        PC_CORE_API ~RhiFrameBuffer() override = default;

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

    protected:
        uint32_t m_Width;

        uint32_t m_Height;
    };

END_PCCORE
