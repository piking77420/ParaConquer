#include "rendering/frame_buffer.hpp"

#include "rendering/render_harware_interface/RHI.hpp"


PC_CORE::FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& _frameBufferCreateInfo)
{
    RHIFrameBufferCreateInfo frameBufferCreateInfo;
    frameBufferCreateInfo.imageViewHandles.resize(_frameBufferCreateInfo.renderTargetCount);
    for (size_t i = 0; i < _frameBufferCreateInfo.renderTargetCount; i++)
    {
        frameBufferCreateInfo.imageViewHandles[i] = _frameBufferCreateInfo.renderTargets[i].GetImageViewHandle();
    }
    frameBufferCreateInfo.renderPassHandle = _frameBufferCreateInfo.renderPass->GetHandle();

    frameBufferCreateInfo.width = _frameBufferCreateInfo.width;
    frameBufferCreateInfo.height = _frameBufferCreateInfo.height;
    frameBufferCreateInfo.layers = _frameBufferCreateInfo.layers;

    m_FrameBufferHandle = RHI::GetInstance().CreateFrameBuffer(frameBufferCreateInfo);
}

PC_CORE::FrameBuffer::~FrameBuffer()
{
    Destroy();
}

PC_CORE::FrameBufferHandle PC_CORE::FrameBuffer::GetHandle() const
{
    return m_FrameBufferHandle;
}

void PC_CORE::FrameBuffer::Destroy()
{
    
    if (m_FrameBufferHandle != NULL_HANDLE)
    {
        RHI::GetInstance().DestroyFrameBuffer(m_FrameBufferHandle);
        m_FrameBufferHandle = NULL_HANDLE;
    }
}
