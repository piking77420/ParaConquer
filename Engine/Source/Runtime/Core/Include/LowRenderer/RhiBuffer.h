#pragma once

#include "RhiResource.hpp"

BEGIN_PCCORE

    class CommandList;

    class RhiBuffer : public RhiResourceT<RhiBuffer>
    {
    public:

        enum struct BufferUpdateRate: uint8_t
        {
            Static,     // Written once
            PerFrame,   // Updated once per frame
            Dynamic     // Updated many times per frame
        };

        enum IndexFormat : uint8_t
        {
            Uint8 = 1,
            Uint16 = 2,
            Uint32 = 4
        };
        static_assert(static_cast<uint8_t>(IndexFormat::Uint8) == sizeof(uint8_t), "Size of Uint8");
        static_assert(static_cast<uint8_t>(IndexFormat::Uint16) == sizeof(uint16_t), "Size of Uint32");
        static_assert(static_cast<uint8_t>(IndexFormat::Uint32) == sizeof(uint32_t), "Size of Uint16");
        REFLECT(IndexFormat)
    
        enum BufferUsageFlagBits : uint32_t
        {
            NoneBufferUsageFlag = 0,
            Uniform       = 1 << 0,
            Vertex        = 1 << 1,
            Index         = 1 << 2,
            ShaderStorage = 1 << 3,
            TransferSrc   = 1 << 4,
            TransferDst   = 1 << 5,
            RayTracing    = 1 << 6,
        };
        using BufferUsageFlag = uint32_t;

    
        REFLECT(BufferUsageFlagBits)

        enum BufferBackingStrategy 
        {
            SingleBuffer,        // One backend buffer, no per-frame duplication
            CpuVisibleRing,      // One CPU-visible buffer with per-frame offsets
            PerFrameBuffers,     // One backend buffer per frame (duplication)
            StagedUpload,        // GPU-only buffer + staging buffer(s)
            Readback             // CPU-visible buffer for GPU→CPU reads
        };
    
        static constexpr const char* DynamicBufferKey = "DynamicObject";
       
        DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)

        PC_CORE_API RhiBuffer(Rhi& _Rhi);
    
        PC_CORE_API ~RhiBuffer() override = default;

        PC_CORE_API virtual bool Build() override;
    
        PC_CORE_API virtual bool UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes) = 0;
    
        PC_CORE_API virtual char* BeginFullDynamicBufferUpdateForCurrentFrame() = 0;

        PC_CORE_API virtual void EndBufferUpdate() = 0;

        PC_CORE_API virtual char* BeginBufferUpdateForCurrentFrame() = 0;

        RhiBuffer& SetSizeInBytes(size_t _SizeInByte)
        {
            m_SizeInByte = _SizeInByte;
            return *this;
        }

        RhiBuffer& SetUsage(BufferUsageFlag _Usage)
        {
            m_Usage = _Usage;
            return *this;
        }

        RhiBuffer& SetBufferUpdateRate(BufferUpdateRate _BufferUpdateRate)
        {
            m_BufferUpdateRate = _BufferUpdateRate;
            return *this;
        }

        RhiBuffer& SetObjectSize(size_t _size)
        {
            m_ObjectSize = _size;
            return *this;
        }

        RhiBuffer& SetMaxObjectPerFrame(size_t _MaxObjectPerFrame)
        {
            m_MaxObjectPerFrame = _MaxObjectPerFrame;
            return *this;
        }

        size_t GetSizeInByte() const
        {
            return m_SizeInByte;
        }
    
        BufferUsageFlag GetUsage() const
        {
            return m_Usage;
        }
    
        BufferUpdateRate GetBufferUpdateRate() const
        {
            return m_BufferUpdateRate;
        }
    
        BufferBackingStrategy GetBufferBackingStrategy() const
        {
            return m_BufferBackingStrategy;
        }

        size_t GetObjectSize() const
        {
            return m_ObjectSize;
        }

        size_t GetMaxObjectPerFrame() const
        {
            return m_MaxObjectPerFrame;
        }

        size_t GetStride() const
        {
            return m_Stride;
        }

        size_t GetFrameStride() const
        {
            return m_FrameStride;
        }

    protected:
        uint32_t m_SizeInByte = 0u;

        BufferUsageFlag m_Usage = 0u;

        BufferUpdateRate m_BufferUpdateRate = {};

        BufferBackingStrategy m_BufferBackingStrategy{};

        size_t m_ObjectSize = 0;

        size_t m_MaxObjectPerFrame = 0;

        size_t m_Stride = 0;
        
        size_t m_FrameStride = 0;

        void ChooseBackingStrategy();
};

END_PCCORE
