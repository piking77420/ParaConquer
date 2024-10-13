#pragma once

#include "gpu_buffer.hpp"
#include "core_header.hpp"

BEGIN_PCCORE

class IndexBuffer : public GPUBuffer
{
public:

    IndexBuffer(IndexBuffer&& _other) noexcept;
   
    IndexBuffer& operator=(IndexBuffer&& _other) noexcept;
    
    IndexBuffer(CommandPool* _transfertPool, const std::vector<uint32_t>& _indicies);
    
    IndexBuffer();
    
    ~IndexBuffer();

    inline uint32_t GetNbrOfIndicies()
    {
        return m_IndiciesCount;
    }
    
private:
    uint32_t m_IndiciesCount = 0;
};

END_PCCORE