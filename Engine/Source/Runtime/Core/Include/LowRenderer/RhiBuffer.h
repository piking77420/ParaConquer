#pragma once

#include "RhiResource.hpp"

BEGIN_PCCORE

    class CommandList;

    class RhiBuffer : public RhiResourceT<RhiBuffer>
    {
    public:
        enum IndexFormat : uint8_t
        {
            Uiunt8 = 1,
            Uint16 = 2,
            Uint32 = 4
        };
        static_assert(static_cast<uint8_t>(IndexFormat::Uiunt8) == sizeof(uint8_t), "Size of Uint8");
        static_assert(static_cast<uint8_t>(IndexFormat::Uint16) == sizeof(uint16_t), "Size of Uint32");
        static_assert(static_cast<uint8_t>(IndexFormat::Uint32) == sizeof(uint32_t), "Size of Uint16");
        REFLECT(IndexFormat)
    
        enum BufferUsageFlagBits : uint32_t
        {
            None          = 0,
            Uniform       = 1 << 0,
            Vertex        = 1 << 1,
            Index         = 1 << 2,
            ShaderStorage = 1 << 3,
            TransferSrc   = 1 << 4,
            TransferDst   = 1 << 5,
            RayTracing    = 1 << 6,
            MeshShader    = 1 << 7,
        };
        using BufferUsageFlag = uint32_t;

        static_assert(MeshShader <= (1u << 31),
              "BufferUsage flags exceed uint32_t bit capacity");
    
        REFLECT(BufferUsageFlagBits)
    
        struct RhiBufferDescriptor
        {
            
        };
    
        DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)

        PC_CORE_API RhiBuffer(Rhi& _Rhi);
    
        PC_CORE_API ~RhiBuffer() override = default;
    
        PC_CORE_API virtual void UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes) = 0;
    
        PC_CORE_API virtual char* BeginFullDynamicBufferUpdateForCurrentFrame() = 0;

        PC_CORE_API virtual void EndFullDynamicBufferUpdateForCurrentFrame() = 0;

        RhiBuffer& SetSize(size_t _SizeInByte)
        {
            m_SizeInByte = _SizeInByte;
            return *this;
        }

        RhiBuffer& SetUsage(BufferUsageFlag _Usage)
        {
            m_Usage = _Usage;
            return *this;
        }

        size_t GetSize() const
        {
            return m_SizeInByte;
        }
    
        BufferUsageFlag GetUsage() const
        {
            return m_Usage;
        }
    
        size_t GetNbrOfInFlightResource() const
        {
            return GetNbrOfHandle(m_MemoryUsage);
        }
    
    protected:
        uint32_t m_SizeInByte = 0u;

        BufferUsageFlag m_Usage = 0u;
};

END_PCCORE
