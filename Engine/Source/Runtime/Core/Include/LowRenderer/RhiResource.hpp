#pragma once

#include "RhiObject.hpp"

namespace PC_CORE
{

class RhiResource : public RhiObjectT<RhiResource>
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
        RenderTarget,
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

    PC_CORE_API explicit RhiResource(Rhi& _Rhi);

    PC_CORE_API virtual ~RhiResource() = default;

    DEFAULT_COPY_MOVE_OPERATIONS(RhiResource)

protected:
    bool m_AllowCpuAcces = false;

    static uint32_t GetNbrOfHandle(MemoryUsage _memoryUsage)
    {
        switch (_memoryUsage)
        {
        case MemoryUsage::None:
        case MemoryUsage::Count:
            assert(false);
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

template <typename T>
class RhiResourceT : public RhiResource
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiResourceT);

    ~RhiResourceT() override = default;

    explicit RhiResourceT(Rhi& _Rhi)
        : RhiResource(_Rhi)
    {
    }

    // Setter

    T& SetMemoryUsage(MemoryUsage _MemoryUsage)
    {
        static_assert(std::is_base_of_v<RhiResource, T>, "T must be an RhiResource");

        m_MemoryUsage = _MemoryUsage;
        return reinterpret_cast<T&>(*this);
    }

    T& SetAllowCpuAcess(bool _AllowCpuAcess)
    {
        static_assert(std::is_base_of_v<RhiResource, T>, "T must be an RhiResource");

        m_AllowCpuAcces = _AllowCpuAcess;
        return reinterpret_cast<T&>(*this);
    }

    // Getter
    MemoryUsage GetMemoryUsage() const
    {
        return m_MemoryUsage;
    }

    bool GetAllowCpuAcces() const
    {
        return m_AllowCpuAcces;
    }

    size_t GetNbrOfInFlightResource() const
    {
        return GetNbrOfHandle(m_MemoryUsage);
    }


private:

};

}

using RhiResourceState = PC_CORE::RhiResource::State;
using RhiMemoryUsage = PC_CORE::RhiResource::MemoryUsage;
