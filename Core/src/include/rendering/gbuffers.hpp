#pragma once

#include <memory>

#include "rendering_typedef.h"
#include "low_renderer/frame_buffer.hpp"
#include "resources/texture_2d.hpp"

BEGIN_PCCORE

class Gbuffers
{
public:

    PC_CORE_API std::shared_ptr<FrameBuffer> GetGbufferFrameBuffer() const;

    PC_CORE_API std::shared_ptr<FrameBuffer> GetForwardFrameBuffer() const;

    PC_CORE_API ShaderProgramDescriptorSets* GetDescritptorSet() const
    {
        return m_DescriptorSets;
    }
    
    PC_CORE_API void HandleResize(Tbx::Vector2i _targetSize , std::shared_ptr<RhiRenderPass> _forwardPass, std::shared_ptr<RhiRenderPass> _gbufferPass);

    PC_CORE_API Gbuffers(Tbx::Vector2i _size, std::shared_ptr<RhiRenderPass> _renderPass);

    PC_CORE_API Gbuffers() = default;

    PC_CORE_API ~Gbuffers() = default;

    PC_CORE_API Tbx::Vector2i GetSize() const
    {
        return m_size;
    }

    PC_CORE_API std::shared_ptr<Texture2D> GetTexture(GbufferType type) const;

    PC_CORE_API Texture2D& GetImage()
    {
        return m_Image;
    }


    
private:
    std::array<std::shared_ptr<Texture2D>, static_cast<uint8_t>(GbufferType::Count)> m_Gbuffers;

    Texture2D m_Image;

    std::shared_ptr<FrameBuffer> m_GbufferFrameBuffer;

    std::shared_ptr<FrameBuffer> m_ForwardFrameBuffer;

    Tbx::Vector2i m_size;

    ShaderProgramDescriptorSets* m_DescriptorSets;

    PC_CORE_API void CreateGBuffers();
};
END_PCCORE