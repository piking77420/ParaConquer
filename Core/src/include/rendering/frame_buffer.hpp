#pragma once

#include "rendering_typedef.h"
#include "render_pass.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
struct FrameBufferCreateInfo
{
    std::vector<Texture*> renderTargets;
    const RenderPass* renderPass;

    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    layers;
};

class FrameBuffer
{
public:
    PC_CORE_API FrameBuffer(FrameBuffer&& _other) noexcept
    {
        Destroy();

        m_FrameBufferHandle = _other.m_FrameBufferHandle;
        _other.m_FrameBufferHandle = NULL_HANDLE;
    }
    
    PC_CORE_API FrameBuffer& operator=(FrameBuffer&& _other) noexcept
    {
        Destroy();

        m_FrameBufferHandle = _other.m_FrameBufferHandle;
        _other.m_FrameBufferHandle = NULL_HANDLE;

        return *this;
    }

    PC_CORE_API FrameBuffer(const FrameBufferCreateInfo& _frameBufferCreateInfo);
    
    PC_CORE_API FrameBuffer() = default;
    
    PC_CORE_API ~FrameBuffer();

    PC_CORE_API FrameBufferHandle GetHandle() const;

private:
    FrameBufferHandle m_FrameBufferHandle;

    void Destroy();
};

END_PCCORE