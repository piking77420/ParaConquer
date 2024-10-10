#include "rendering/render_harware_interface/command_buffer.h"

#include "rendering/render_harware_interface/RHI.hpp"

void PC_CORE::CommandBuffer::BindVertexBuffer(const VertexBuffer& _buffer)
{
    RHI::GetInstance()->BindVertexBuffer(handle, _buffer.handleId);
}
