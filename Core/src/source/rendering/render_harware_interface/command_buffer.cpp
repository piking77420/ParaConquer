#include "rendering/render_harware_interface/command_buffer.h"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& _commandBufferCreateInfo)
{
    RHI::GetInstance()->AllocateCommandBuffer(m_Handle,_commandBufferCreateInfo)
}

PC_CORE::CommandBuffer::CommandBuffer()
{
    
}

PC_CORE::CommandBuffer::~CommandBuffer()
{
}
