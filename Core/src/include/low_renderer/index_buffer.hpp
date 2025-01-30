#pragma once

#include "core_header.hpp"
#include "gpu_buffer.hpp"

BEGIN_PCCORE
class IndexBuffer : public GpuBuffer
{
public:


    DEFAULT_COPY_MOVE_OPERATIONS(IndexBuffer)

    IndexFormat GetIndexFormat() const;

    uint32_t GetIndexCount() const;

    IndexBuffer(const uint32_t* _indices, size_t _count);

    IndexBuffer(const uint16_t* _indices, size_t _count);

    IndexBuffer(const uint8_t* _indices, size_t _count);
    
    IndexBuffer() = default;

    ~IndexBuffer() = default;

private:
    IndexFormat m_IndexFormat = IndexFormat::Uint16;
    
    uint32_t m_IndexCount = 0;

};

END_PCCORE