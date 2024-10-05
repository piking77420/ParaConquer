#pragma once

#include "RHI.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

template <GPU_BUFFER_USAGE T>
class GpuBuffer
{
public:
    void Alloc(size_t size, const void* _data);
    
    GpuBuffer() = default;
    
    ~GpuBuffer() = default;
    
private:
    size_t m_Size = 0;

    
};

template <GPU_BUFFER_USAGE T>
void GpuBuffer<T>::Alloc(size_t size, const void* _data)
{
    m_Size = size;
    RHI::GetInstance()->BufferData(size, _data, T);
}


using VertexBuffer = GpuBuffer<GPU_BUFFER_USAGE::VERTEX>;

END_PCCORE
