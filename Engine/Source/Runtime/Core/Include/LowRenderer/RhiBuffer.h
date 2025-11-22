#pragma once

#include "RhiResource.hpp"

BEGIN_PCCORE
    class RhiBuffer : public RhiResource
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)

        PC_CORE_API RhiBuffer() = default;

        PC_CORE_API RhiBuffer(size_t _sizeInByte,
                              MemoryLocalisation _visibility, MemoryUsage _usage) : m_MemoryUsage(_usage),
            m_MemoryVisibility(_visibility), m_SizeInByte(_sizeInByte)
        {
        }

        PC_CORE_API ~RhiBuffer() override = default;

        virtual void MapData(void** _ptr)
        {
            assert(
                m_MemoryVisibility == MemoryLocalisation::CpuToGpu ||
                m_MemoryVisibility == MemoryLocalisation::CpuOnly
            );
        }

        virtual void UnmapData()
        {
        }


        MemoryUsage GetMemoryUsage() const
        {
            return m_MemoryUsage;
        }

        MemoryLocalisation GetMemoryVisibility() const
        {
            return m_MemoryVisibility;
        }

        size_t GetSize() const
        {
            return m_SizeInByte;
        }

    private:
        MemoryUsage m_MemoryUsage = MemoryUsage::Count;

        MemoryLocalisation m_MemoryVisibility = MemoryLocalisation::Count;

        size_t m_SizeInByte = 0;
    };

END_PCCORE
