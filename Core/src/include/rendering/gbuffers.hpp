#pragma once

#include <memory>

#include "rendering_typedef.h"
#include "low_renderer/frame_buffer.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
enum GbufferType
{
    Albedo,
    Depth,

    Count
};

class Gbuffers
{
public:

    PC_CORE_API std::shared_ptr<FrameBuffer> GetFrameBuffer() const;
    
    PC_CORE_API void HandleResize(Tbx::Vector2i _targetSize, std::shared_ptr<RhiRenderPass> _renderPass);

    PC_CORE_API Gbuffers(Tbx::Vector2i _size, std::shared_ptr<RhiRenderPass> _renderPass);

    PC_CORE_API Gbuffers() = default;

    PC_CORE_API ~Gbuffers() = default;

    PC_CORE_API Tbx::Vector2i GetSize() const
    {
        return m_size;
    }

    PC_CORE_API std::shared_ptr<Texture> GetTexture(GbufferType type) const;
    
private:
    std::array<std::shared_ptr<Texture>, GbufferType::Count> m_gbuffers;

    std::shared_ptr<FrameBuffer> m_FrameBuffer;

    Tbx::Vector2i m_size;

    PC_CORE_API void CreateGBuffers();
};
END_PCCORE