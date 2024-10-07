#pragma once

#include "RHI.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

template <GPU_BUFFER_USAGE T>
class GpuBuffer
{
public:

    GpuBuffer(GpuBuffer&& _otherGpuBuffer) noexcept;
    
    GpuBuffer& operator=(GpuBuffer&& _otherGpuBuffer) noexcept; 
    
    GpuBuffer() = default;
    
    GpuBuffer(size_t size, const void* _data);
    
    ~GpuBuffer();

    bool HasAllocation() const;
    
    void Bind(CommandBufferHandle _commandBufferHandle) const;

private:
    size_t m_Size = 0;

    GPUBufferHandle m_HandleId = nullptr;
};

template <GPU_BUFFER_USAGE T>
GpuBuffer<T>::GpuBuffer(GpuBuffer&& _otherGpuBuffer) noexcept
{
    m_Size = _otherGpuBuffer.m_Size;
    m_HandleId = _otherGpuBuffer.m_HandleId;

    _otherGpuBuffer.m_HandleId = nullptr;
    _otherGpuBuffer.m_Size = 0;
}

template <GPU_BUFFER_USAGE T>
GpuBuffer<T>& GpuBuffer<T>::operator=(GpuBuffer&& _otherGpuBuffer) noexcept
{
    m_Size = _otherGpuBuffer.m_Size;
    m_HandleId = _otherGpuBuffer.m_HandleId;

    _otherGpuBuffer.m_HandleId = nullptr;
    _otherGpuBuffer.m_Size = 0;

    return *this;
}

template <GPU_BUFFER_USAGE T>
GpuBuffer<T>::GpuBuffer(size_t size, const void* _data)
{
    m_Size = size;
    m_HandleId = RHI::GetInstance()->BufferData(size, _data, T);
}

template <GPU_BUFFER_USAGE T>
GpuBuffer<T>::~GpuBuffer()
{
    if (HasAllocation())
    {
        RHI::GetInstance()->DestroyBuffer(m_HandleId);
        m_HandleId = nullptr;
        m_Size = 0;
    }
}

template <GPU_BUFFER_USAGE T>
bool GpuBuffer<T>::HasAllocation() const
{
    return m_HandleId != nullptr;
}

template <GPU_BUFFER_USAGE T>
void GpuBuffer<T>::Bind(CommandBufferHandle _commandBufferHandle) const
{
    RHI::GetInstance()->BindBuffer(_commandBufferHandle, m_HandleId);   
}


using VertexBuffer = GpuBuffer<GPU_BUFFER_USAGE::VERTEX>;
using IndexBuffer = GpuBuffer<GPU_BUFFER_USAGE::INDEX>;
using UniformBuffer = GpuBuffer<GPU_BUFFER_USAGE::UNIFORM>;
using ShaderStorageBuffer = GpuBuffer<GPU_BUFFER_USAGE::SHADER_STORAGE>;

END_PCCORE
