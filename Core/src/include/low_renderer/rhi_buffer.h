#pragma once

#include "rhi_resource.hpp"

BEGIN_PCCORE

class RhiBuffer : public RhiResource
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)
    
    PC_CORE_API RhiBuffer() = default;

    PC_CORE_API RhiBuffer(size_t _sizeInByte, PC_CORE::BufferMemoryUsage _usage) : m_MemoryUsage(_usage) , m_SizeInByte(_sizeInByte)
    {
        
    }

    PC_CORE_API virtual ~RhiBuffer() = default;

    virtual void MapData(void** _ptr)
    {
        
    }
    virtual void UnmapData()
    {
        
    }

    size_t GetSize() const
    {
        return m_SizeInByte;
    }

    PC_CORE::BufferMemoryUsage GetMemoryUsage() const
    {
        return m_MemoryUsage;
    }

private:
    PC_CORE::BufferMemoryUsage m_MemoryUsage = PC_CORE::BufferMemoryUsage::Count;

    size_t m_SizeInByte = 0;
};

END_PCCORE