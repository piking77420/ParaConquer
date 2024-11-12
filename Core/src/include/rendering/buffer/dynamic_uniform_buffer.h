#pragma once
#include "gpu_buffer.hpp"

BEGIN_PCCORE

class DynamicUniformBuffer : PC_CORE::GPUBuffer
{
public:

    DynamicUniformBuffer(DynamicUniformBuffer&& _other) noexcept;
   
    DynamicUniformBuffer& operator=(DynamicUniformBuffer&& _other) noexcept;

    DynamicUniformBuffer(size_t _size);

    DynamicUniformBuffer();
    
    ~DynamicUniformBuffer();

    void Update(size_t _size, size_t _offset, const void* _data);

    
};


using DUniformBuffer = DynamicUniformBuffer;
END_PCCORE