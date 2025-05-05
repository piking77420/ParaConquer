#pragma once

#include "gpu_buffer.hpp"
#include "low_renderer/uniform_buffer.hpp"

BEGIN_PCCORE

class UniformBuffer : public PC_CORE::GpuBuffer
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(UniformBuffer)

    PC_CORE_API void Update(void* _data, size_t _size);

    PC_CORE_API UniformBuffer(void* _data, size_t _size);
    
    PC_CORE_API UniformBuffer() = default;
    
    PC_CORE_API ~UniformBuffer();

    PC_CORE_API const std::shared_ptr<GpuHandle>* GetHandle() const;

    PC_CORE_API std::shared_ptr<GpuHandle>* GetHandle();

private:
    std::array<void*, MAX_FRAMES_IN_FLIGHT> m_MappedMemory;
};

END_PCCORE