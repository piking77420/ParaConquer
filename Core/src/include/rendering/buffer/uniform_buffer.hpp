#pragma once

#include "gpu_buffer.hpp"
#include "low_renderer/rhi_uniform_buffer.hpp"

BEGIN_PCCORE
    class PC_CORE_API UniformBuffer final: public GpuBuffer
{
public:
    std::shared_ptr<RhiResource> GetRhiHandle() const override
    {
        return m_RhiBuffer;
    }

    void Update(void* _data, size_t _size);

    explicit UniformBuffer(void* _data, size_t _size, BufferMemoryUsage _usage);

    explicit UniformBuffer() = default;

    ~UniformBuffer() override = default;

private:
    std::shared_ptr<RhiBuffer> m_RhiBuffer;
    
};

END_PCCORE