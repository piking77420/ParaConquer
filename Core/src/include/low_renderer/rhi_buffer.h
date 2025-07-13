#pragma once

#include "rhi_resource.hpp"

BEGIN_PCCORE

class RhiBuffer : public RhiResource
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)
    
    PC_CORE_API RhiBuffer() = default;

    PC_CORE_API RhiBuffer(size_t _sizeInByte,
        PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage _usage) : m_MemoryUsage(_usage) , m_MemoryVisibility(_visibility),  m_SizeInByte(_sizeInByte)
    {
        
    }

    PC_CORE_API virtual ~RhiBuffer() = default;

    virtual void MapData(void** _ptr)
    {
        assert(
            m_MemoryVisibility == MemoryLocalisation::CPU_To_GPU ||
            m_MemoryVisibility == MemoryLocalisation::CPU_Only
        );

    }
    virtual void UnmapData()
    {
        
    }

    
    PC_CORE::MemoryUsage GetMemoryUsage() const
    {
        return m_MemoryUsage;
    }
    
    PC_CORE::MemoryLocalisation GetMemoryVisibility() const
    {
        return m_MemoryVisibility;
    }

    size_t GetSize() const
    {
        return m_SizeInByte;
    }

private:
    PC_CORE::MemoryUsage m_MemoryUsage = PC_CORE::MemoryUsage::Count;

    PC_CORE::MemoryLocalisation m_MemoryVisibility = PC_CORE::MemoryLocalisation::Count;

    size_t m_SizeInByte = 0;
};

END_PCCORE