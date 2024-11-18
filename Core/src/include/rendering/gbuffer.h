#pragma once

#include "rendering_typedef.h"
#include "render_pass.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
    class FrameBuffer;

    class Gbuffer
{
public:

    PC_CORE_API Gbuffer& operator=(Gbuffer&& _other);

    PC_CORE_API bool HandleResize(Tbx::Vector2i _size, const RenderPass& _renderPass); 
    
    PC_CORE_API Gbuffer();

    PC_CORE_API ~Gbuffer() = default;

private:
    Tbx::Vector2i m_Size;
    
    std::vector<std::array<PC_CORE::Texture, static_cast<size_t>(GBufferType::COUT)>> m_RenderTargets;

    std::vector<PC_CORE::FrameBuffer> m_FrameBuffers;
};

END_PCCORE