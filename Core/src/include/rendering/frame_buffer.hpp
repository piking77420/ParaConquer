#pragma once

#include "rendering_typedef.h"
#include "render_pass.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
struct FrameBufferCreateInfo
{
    std::vector<Texture> imageViewHandles;
    RenderPass renderPass;

    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    layers;
};

class FrameBuffer
{
public:

    FrameBuffer(const FrameBufferCreateInfo& _frameBufferCreateInfo);
    
    FrameBuffer() = default;
    
    ~FrameBuffer();

private:
    FrameBufferHandle m_FrameBufferHandle;
};

END_PCCORE