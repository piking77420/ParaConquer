#include "Resources/StaticMesh.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiResource.hpp"
#include "LowRenderer/RhiResourceUpdate.hpp"
#include "CompactBuffer.hpp"
#include "Rendering/RenderingTypedef.h"

#include "Rendering/Material.hpp"

using namespace PC_CORE;

void StaticMesh::AfterSerialize(Serializer* _serializer) const
{
    PC_LOG("AfterSerialize Static Mesh")
}

void StaticMesh::AfterDeSerialize(Serializer* _serializer)
{
    PC_LOG("AfterDeSerialize Static Mesh")

    CompactBuffer verticiesBuffer;
    CompactBuffer indiciesBuffer;

    _serializer->DeSerializeCompactBuffer("StaticMeshRenderData Vertex", &verticiesBuffer);
    _serializer->DeSerializeCompactBuffer("StaticMeshRenderData Indicies", &indiciesBuffer);

    /*
    if (verticiesBuffer.GetCompressedDataSize() != 0 && indiciesBuffer.GetCompressedDataSize() != 0)
    {
        std::vector<StaticMeshVertex> verticiesRaw = verticiesBuffer.ExtractData<StaticMeshVertex>();
        std::vector<uint32_t> indiciesRaw = indiciesBuffer.ExtractData<uint32_t>();

        
        VertexBuffer = VertexBuffer(Name + "VertexBuffer" ,verticiesRaw.size(), sizeof(StaticMeshVertex), PC_CORE::RhiResource::MemoryUsage::Static);
        VertexBuffer->Build();
        VertexBuffer->UploadData(verticiesRaw.data(), verticiesRaw.size() * sizeof(StaticMeshVertex));
        
        IndexBuffer = IndexBuffer(Name + "VertexBuffer" , indiciesRaw.size(), RhiBuffer::IndexFormat::Uint32 , PC_CORE::RhiResource::MemoryUsage::Static);
        IndexBuffer->Build();
        IndexBuffer->UploadData(indiciesRaw.data(), indiciesRaw.size() * sizeof(StaticMeshVertex));

        if (m_HallowCpuAcces)
        {
            m_RenderData.Vertices = std::move(verticiesRaw);
            m_RenderData.Indices = std::move(indiciesRaw);
        }
    }*/
}

StaticMesh& StaticMesh::SetBaseMaterials(const std::vector<ObjectPtr<Rendering::Material>>& _Material)
{
    m_BaseMaterials.resize(_Material.size());
    for (size_t i = 0; i < _Material.size(); i++)
    {
        m_BaseMaterials[i] = _Material[i];
    }

    return *this;
}

StaticMesh::StaticMesh(std::string _Name, const StaticMeshData& _StaticMeshData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshData(_StaticMeshData)
{
    DYNAMIC_REFLECT_INIT

    InitFromRenderData(_StaticMeshData, _Branch);
}

StaticMesh::StaticMesh(std::string _Name, StaticMeshData&& _StaticMeshData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshData(std::move(_StaticMeshData))
{
    DYNAMIC_REFLECT_INIT

    InitFromRenderData(_StaticMeshData, _Branch);
}

const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& StaticMesh::GetBaseMaterial() const
{
    return m_BaseMaterials;
}

void StaticMesh::InitFromRenderData(const StaticMeshData& _StaticMeshData, RHI::ResourceUpdateBranch* _Branch)
{
    Rhi& rhi = App::Instance->RenderHarwareInteface;
    const StaticMeshRenderData& RenderData = _StaticMeshData.RenderData;

    m_Aabb = _StaticMeshData.AABB;
    m_IsBuildForMeshlet = RenderData.Meshlets.size() > 0;

    if (m_IsBuildForMeshlet)
    {
        assert(!RenderData.MeshletVertexTrianglesIndex.empty() || !RenderData.MeshletTriangles.empty());
    }

    m_MeshSectionGpu.resize(_StaticMeshData.MeshLods.size());

    for (size_t i = 0; i < m_MeshSectionGpu.size(); i++)
        InitMeshSectionGpu(_StaticMeshData, i, _Branch);
    

    if (!m_HallowCpuAcces)
    {
        m_StaticMeshData.RenderData.~StaticMeshRenderData(); // free 
    }

    if (m_MeshSectionGpu.size() > 1)
    {
        m_LODThreshold.clear();
        const size_t lodCount = m_MeshSectionGpu.size();
        m_LODThreshold.reserve(lodCount - 1);
        double base = 0.5;
        for (size_t i = 0; i < lodCount - 1; ++i)
        {
            m_LODThreshold.emplace_back(base);
            base *= 0.5;
        }
    }
}

void StaticMesh::InitMeshSectionGpu(const StaticMeshData& _StaticMeshData, size_t LodIndex, RHI::ResourceUpdateBranch* _Branch)
{
    MeshSectionGpu& MeshSectionGpu = m_MeshSectionGpu[LodIndex];
    Rhi& rhi = App::Instance->RenderHarwareInteface;
    
    const StaticMeshRenderData& RenderData = _StaticMeshData.RenderData;
    const MeshLOD& MeshLod = _StaticMeshData.MeshLods[LodIndex];
    const MeshDataDescriptor& MeshLodDescritptor = _StaticMeshData.MeshLods[LodIndex].Descriptor;

    if (MeshLodDescritptor.IndicesCount > 0 && MeshLodDescritptor.VertexCount > 0)
    {
        // VertexBuffer
        MeshSectionGpu.VertexBuffer = VertexBuffer(rhi);
        MeshSectionGpu.VertexBuffer
            .SetVerticiesCount(MeshLodDescritptor.VertexCount)
            .SetVerticiesSize(sizeof(StaticMeshVertex))
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetSizeInBytes(MeshLodDescritptor.VertexCount * sizeof(StaticMeshVertex))
            .SetUsage(m_IsBuildForMeshlet ? RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::ShaderStorage : RhiBuffer::BufferUsageFlagBits::Vertex)
            .SetName(Name + std::format(" Vertex Buffer LOD {}", LodIndex))
            .Build();


        MeshSectionGpu.IndexBuffer = IndexBuffer(rhi);
        MeshSectionGpu.IndexBuffer
            .SetIndexCount(MeshLodDescritptor.IndicesCount)
            .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetSizeInBytes(MeshLodDescritptor.IndicesCount * static_cast<size_t>(RhiBuffer::IndexFormat::Uint32))
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Index)
            .SetName(Name + std::format(" Index Buffer LOD {}", LodIndex))
            .Build();


        const StaticMeshVertex* VerticiesStart = m_StaticMeshData.RenderData.Vertices.data() + MeshLodDescritptor.VertexOffset;
        const uint32_t* IndexBufferStart = m_StaticMeshData.RenderData.Indices.data() + MeshLodDescritptor.IndicesOffset;
        _Branch
            ->BufferUpload(*MeshSectionGpu.VertexBuffer.Get(), VerticiesStart, MeshSectionGpu.VertexBuffer->GetSizeInByte())
            .BufferUpload(*MeshSectionGpu.IndexBuffer.Get(), IndexBufferStart, MeshSectionGpu.IndexBuffer->GetSizeInByte());
    }

    if (m_IsBuildForMeshlet)
    {
        MeshSectionGpu.MeshletVertexTriangleIndexBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletVertexTriangleIndexBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescritptor.MeshletVertexTrianglesIndexCount * sizeof(RenderData.MeshletVertexTrianglesIndex[0]))
            .SetName(Name + std::format("Meshlet Vertex Buffer LOD {}", LodIndex))
                .Build();

        const uint32_t* MesletVertexTriangleStart = RenderData.MeshletVertexTrianglesIndex.data() + MeshLodDescritptor.MeshletVertexTrianglesIndexOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletVertexTriangleIndexBuffer.get(), MesletVertexTriangleStart, MeshSectionGpu.MeshletVertexTriangleIndexBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletTriangleBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletTriangleBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescritptor.MeshletTrianglesCount * sizeof(RenderData.MeshletTriangles[0]))
            .SetName(Name + std::format("Meshlet Triangles Buffer LOD {}", LodIndex))
                .Build();

        const uint32_t * MeshletTriangleStart = RenderData.MeshletTriangles.data() + MeshLodDescritptor.MeshletTrianglesOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletTriangleBuffer, MeshletTriangleStart, MeshSectionGpu.MeshletTriangleBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescritptor.MeshetCount * sizeof(RenderData.Meshlets[0]))
            .SetName(Name + std::format("Meshlet Buffer LOD {}", LodIndex))
            .Build();

        const Meshlet* MeshletStart = RenderData.Meshlets.data() + MeshLodDescritptor.MeshetOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletBuffer, MeshletStart, MeshSectionGpu.MeshletBuffer->GetSizeInByte());


        // AABB
        struct alignas(16) GPUAABB
        {
            float min[4];
            float max[4];
        };

        std::vector<GPUAABB> GPUAabbs;
        GPUAabbs.reserve(MeshLodDescritptor.MeshetCount);
        std::span CpuAABBs(RenderData.MeshletAABB.data() + MeshLodDescritptor.MeshetOffset, MeshLodDescritptor.MeshetCount);
        for (const auto& CpuAABB : CpuAABBs)
        {
            GPUAABB GPUAABB;
            GPUAABB.min[0] = static_cast<float>(CpuAABB.min.x);
            GPUAABB.min[1] = static_cast<float>(CpuAABB.min.y);
            GPUAABB.min[2] = static_cast<float>(CpuAABB.min.z);

            GPUAABB.max[0] = static_cast<float>(CpuAABB.max.x);
            GPUAABB.max[1] = static_cast<float>(CpuAABB.max.y);
            GPUAABB.max[2] = static_cast<float>(CpuAABB.max.z);

            GPUAabbs.emplace_back(GPUAABB);
        }

        MeshSectionGpu.MeshletAABBBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletAABBBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(GPUAabbs.size() * sizeof(GPUAABB))
            .SetName(Name + std::format("Meshlet AABB Buffer LOD {}", LodIndex))
            .Build();

        _Branch->BufferUpload(*MeshSectionGpu.MeshletAABBBuffer, GPUAabbs.data(), MeshSectionGpu.MeshletAABBBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletDescriptor.reset(rhi.CreateDescriptorSet());
        MeshSectionGpu.MeshletDescriptor
            ->BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 0, MeshSectionGpu.VertexBuffer.Get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 1, MeshSectionGpu.MeshletBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 2, MeshSectionGpu.MeshletVertexTriangleIndexBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 3, MeshSectionGpu.MeshletTriangleBuffer.get())
            .SetName(Name + std::format("Meshlet Bindings LOD {}", LodIndex))
            .Build();

        MeshSectionGpu.MeshLetCount = RenderData.Meshlets.size();
    }

  
}

StaticMesh::StaticMesh() 
    : Resource()
{
    DYNAMIC_REFLECT_INIT
}
