#pragma once

#include "gpu_buffer.hpp"

BEGIN_PCCORE

class UniformBuffer : private GPUBuffer
{
public:

    UniformBuffer(UniformBuffer&& _other) noexcept;

    UniformBuffer& operator=(UniformBuffer&& _other) noexcept;

    void Update(size_t _size, size_t _offset, const void* _data);
   
    UniformBuffer(CommandPool* _commandPool, size_t _size);

    UniformBuffer();

    ~UniformBuffer();

private:
    void* m_MapData = nullptr;

};

END_PCCORE