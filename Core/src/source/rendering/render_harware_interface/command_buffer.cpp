#include "rendering/render_harware_interface/command_buffer.h"

#include "rendering/render_harware_interface/RHI.hpp"

void PC_CORE::CommandBuffer::BindVertexBuffer(const VertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount)
{
    RHI::GetInstance().BindVertexBuffer(m_Handle, _firstBinding, _bindingCount ,_vertexBuffer.handleId);
}



void PC_CORE::CommandBuffer::BindIndexBuffer(const IndexBuffer& _indexBuffer)
{
    RHI::GetInstance().BindIndexBuffer(m_Handle, _indexBuffer.handleId);
}
