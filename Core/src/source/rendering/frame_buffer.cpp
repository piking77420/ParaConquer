#include "rendering/frame_buffer.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& _frameBufferCreateInfo)
{
    RHIFrameBufferCreateInfo frameBufferCreateInfo;
    std::vector<ImageViewHandle> imageViewHandles(_frameBufferCreateInfo.imageViewHandles.size());
    for (size_t i = 0; i < _frameBufferCreateInfo.imageViewHandles.size(); i++)
    {
        imageViewHandles[i] = _frameBufferCreateInfo.imageViewHandles[i].GetImageHandle();
    }
    frameBufferCreateInfo.renderPassHandle = _frameBufferCreateInfo.renderPass.GetHandle();

    frameBufferCreateInfo.width = _frameBufferCreateInfo.width;
    frameBufferCreateInfo.height = _frameBufferCreateInfo.height;
    frameBufferCreateInfo.layers = _frameBufferCreateInfo.layers;
}

PC_CORE::FrameBuffer::~FrameBuffer()
{
    if (m_FrameBufferHandle != NULL_HANDLE)
    {
        RHI::GetInstance().DestroyFrameBuffer(m_FrameBufferHandle);
    }
}
