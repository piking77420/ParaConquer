#pragma once

#include "RhiObject.hpp"

namespace PC_CORE
{

class RhiResource : public RhiObjectT<RhiResource>
{
public:
    enum struct MemoryUsage : uint8_t
    {
        StaticGPU,        // GPU-only, immutable
        CPUVisible,       // CPU-visible, persistently mapped
        ReadbackCPU       // GPU → CPU
    };

    enum struct State : uint8_t
    {
        Undefined = 0,

        // Transfer
        CopySrc,
        CopyDst,

        // Buffer and Texture uses
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        VertexShaderResource,
        PixelShaderResource,
        RenderTarget,
        DepthStencilWrite,
        DepthStencilRead,

        // should replace with shader read or shader write 
        // Compute
        ComputeRead,
        ComputeWrite,
        ComputeReadWrite,

        // MeshShader
        MeshShaderResource,
        AmpShaderResource,

        // Presentation
        Present,
    };

    PC_CORE_API explicit RhiResource(Rhi& _Rhi);

    PC_CORE_API virtual ~RhiResource() = default;

protected:
    bool m_AllowCpuAcces = false;

    MemoryUsage m_MemoryUsage = {};

    uint32_t m_NbrOfBackendObject{ 0 };
};

template <typename T>
concept RhiResourceType = std::is_base_of_v<RhiResource, T>;

template <typename T>
class RhiResourceT : public RhiResource
{
public:

    explicit RhiResourceT(Rhi& _Rhi)
        : RhiResource(_Rhi)
    {
    }

    ~RhiResourceT() override = default;
    
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

    size_t GetNbrOfBackendObject() const
    {
        return m_NbrOfBackendObject;
    }
     

private:

};

}

using RhiResourceState = PC_CORE::RhiResource::State;
using RhiMemoryUsage = PC_CORE::RhiResource::MemoryUsage;
