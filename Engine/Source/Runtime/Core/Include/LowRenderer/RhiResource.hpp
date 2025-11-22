#pragma once

#include "RhiObject.hpp"

BEGIN_PCCORE
    class RhiResource : public RhiObject
    {
    public:
        enum struct MemoryUsage : uint8_t
        {
            None,
            Static, // Not modified over its lifetime
            Streamable, // Occasionally modified (e.g., once per frame)
            Dynamic, // Frequently modified (e.g., multiple times per frame)

            Count // Total enum values
        };
        REFLECT(MemoryUsage)
    
        enum struct State
        {
            Undefined = 0,

            // Transfer
            CopySrc,
            CopyDst,

            // Buffer uses
            VertexBuffer,
            IndexBuffer,
            UniformBuffer,

            // Texture uses
            ShaderRead,
            RenderTarget, // TODO HANDLE READ WIRTE ONLY 
            DepthStencilWrite,
            DepthStencilRead,

            // Compute
            ComputeRead,
            ComputeWrite,
            ComputeReadWrite,

            // Presentation
            Present,
        };
        REFLECT(MemoryUsage)

        PC_CORE_API explicit RhiResource(Rhi& _Rhi, const std::string& _name, MemoryUsage memoryUsage);

        PC_CORE_API explicit RhiResource(Rhi& _Rhi, std::string&& _name, MemoryUsage memoryUsage);

        PC_CORE_API virtual ~RhiResource() = default;

        DEFAULT_COPY_MOVE_OPERATIONS(RhiResource)

        inline MemoryUsage GetMemoryUsage() const
        {
            return m_MemoryUsage;
        }


    protected:
    
        static uint32_t GetNbrOfHandle(MemoryUsage _memoryUsage)
        {
            switch (_memoryUsage)
            {
            case MemoryUsage::None:
            case MemoryUsage::Count:
                return 0;
            case MemoryUsage::Static:
            case MemoryUsage::Streamable:
                return 1;
            case MemoryUsage::Dynamic:
                return MaxFramesInFlight;
            }
                
            return static_cast<uint32_t>(-1);
        }
        MemoryUsage m_MemoryUsage = MemoryUsage::None;
    };

END_PCCORE

using RhiResourceState = PC_CORE::RhiResource::State;
