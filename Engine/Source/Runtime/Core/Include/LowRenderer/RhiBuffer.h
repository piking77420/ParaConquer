#pragma once

#include "RhiResource.hpp"

BEGIN_PCCORE

    class CommandList;

    class RhiBuffer : public RhiResource
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
    
        enum BufferUsage : uint32_t
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
        static_assert(MeshShader <= (1u << 31),
              "BufferUsage flags exceed uint32_t bit capacity");
    
        REFLECT(BufferUsage)
    
        struct RhiBufferDescriptor
        {
            uint32_t SizeInByte = 0;
            BufferUsage Usage = BufferUsage::None;
        };
    
        DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)

        PC_CORE_API RhiBuffer(Rhi& _Rhi, const std::string& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage);
    
        PC_CORE_API RhiBuffer(Rhi& _Rhi, std::string&& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage);
    
        PC_CORE_API ~RhiBuffer() override = default;
    
        PC_CORE_API virtual void UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes) = 0;
    
        PC_CORE_API virtual char* BeginFullDynamicBufferUpdateForCurrentFrame() = 0;

        PC_CORE_API virtual void EndFullDynamicBufferUpdateForCurrentFrame() = 0;

        size_t GetSize() const
        {
            return m_RhiBufferDescriptor.SizeInByte;
        }
    
        BufferUsage GetUsage() const
        {
            return m_RhiBufferDescriptor.Usage;
        }
    
        size_t GetNbrOfInFlightResource() const
        {
            return GetNbrOfHandle(m_MemoryUsage);
        }
    
    protected:
        RhiBufferDescriptor m_RhiBufferDescriptor {};
};

END_PCCORE
