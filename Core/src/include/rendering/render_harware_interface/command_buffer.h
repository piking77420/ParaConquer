#pragma once

#include "core_header.hpp"
#include "rhi_typedef.h"
#include "rendering/buffer/index_buffer.hpp"
#include "rendering/buffer/vertex_buffer.hpp"

BEGIN_PCCORE
    class CommandBuffer;

struct CommandBufferCreateInfo
{
    CommandBuffer* commandBufferPtr = nullptr;
    uint32_t commandBufferCount = 0;
    CommandBufferlevel commandBufferlevel;
};
    
class CommandBuffer
{
public:
    CommandBuffer() = default;

    ~CommandBuffer() = default;

    void BindVertexBuffer(const VertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount);

    void BindIndexBuffer(const IndexBuffer& _indexBuffer);

    inline GPUObjectHandle GetHandle() const
    {
        return m_Handle;
    }

private:
    GPUObjectHandle m_Handle = nullptr;
};
static_assert(sizeof(CommandBuffer) == 8, "CommandBuffer should be 8 bytes long");




END_PCCORE