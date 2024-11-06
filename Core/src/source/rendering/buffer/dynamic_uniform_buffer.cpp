#include "rendering/buffer/dynamic_uniform_buffer.h"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::DynamicUniformBuffer::DynamicUniformBuffer(DynamicUniformBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;

    _other.handleId = nullptr;
    _other.size = 0;
}

PC_CORE::DynamicUniformBuffer& PC_CORE::DynamicUniformBuffer::operator=(DynamicUniformBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;

    _other.handleId = nullptr;
    _other.size = 0;

    return *this;
}

PC_CORE::DynamicUniformBuffer::DynamicUniformBuffer(size_t _size)
{
    RHI::GetInstance().BufferData(size, GPU_BUFFER_USAGE::DYNAMIC_UNIFORM);
}


PC_CORE::DynamicUniformBuffer::~DynamicUniformBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    size = 0;
    RHI::GetInstance().DestroyBuffer(handleId);
    handleId = nullptr;
}

void PC_CORE::DynamicUniformBuffer::Update(size_t _size, size_t _offset, const void* _data)
{
    
}
