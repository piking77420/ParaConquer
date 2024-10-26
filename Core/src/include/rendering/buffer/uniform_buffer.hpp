#pragma once

#include "gpu_buffer.hpp"

BEGIN_PCCORE

class UniformBuffer : GPUBuffer
{
public:

    UniformBuffer(UniformBuffer&& _other) noexcept;

    UniformBuffer& operator=(UniformBuffer&& _other) noexcept;

    inline size_t GetSize() const noexcept
    {
        return size;
    }

    void Update(size_t _size, size_t _offset, const void* _data);

    DescriptorBufferInfo AsDescriptorBufferInfo(uint32_t _offset) const;
   
    UniformBuffer(CommandPool* _commandPool, size_t _size);

    UniformBuffer();

    ~UniformBuffer();

private:
    void* m_MapData = nullptr;

};

END_PCCORE