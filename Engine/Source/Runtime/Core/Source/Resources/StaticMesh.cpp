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

    InitFromRenderData(_StaticMeshData.RenderData, _Branch);
}

StaticMesh::StaticMesh(std::string _Name, StaticMeshData&& _StaticMeshData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshData(std::move(_StaticMeshData))
{
    DYNAMIC_REFLECT_INIT

    InitFromRenderData(_StaticMeshData.RenderData, _Branch);
}

const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& StaticMesh::GetBaseMaterial() const
{
    return m_BaseMaterials;
}

void StaticMesh::InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
{
    Rhi& rhi = App::Instance->RenderHarwareInteface;

    m_IsBuildForMeshlet = _StaticMeshRenderData.Meshlets.size() > 0;

    if (m_IsBuildForMeshlet)
    {
        assert(!_StaticMeshRenderData.MeshletVertexTrianglesIndex.empty() || !_StaticMeshRenderData.MeshletTriangles.empty());
    }

    m_MeshSectionGpu.resize(_StaticMeshRenderData.LODSDescriptor.size());

    for (size_t i = 0; i < m_MeshSectionGpu.size(); i++)
    {
        InitMeshSectionGpu(_StaticMeshRenderData, i, _Branch);
    }

    if (!m_HallowCpuAcces)
    {
        m_StaticMeshData.RenderData.~StaticMeshRenderData(); // free 
    }
}

void StaticMesh::InitMeshSectionGpu(const StaticMeshRenderData& _StaticMeshRenderData, size_t LodIndex, RHI::ResourceUpdateBranch* _Branch)
{
    const MeshDataDescriptor& MeshLodDescriptor = _StaticMeshRenderData.LODSDescriptor[LodIndex];
    MeshSectionGpu& MeshSectionGpu = m_MeshSectionGpu[LodIndex];

    Rhi& rhi = App::Instance->RenderHarwareInteface;

    if (m_IsBuildForMeshlet)
    {
        MeshSectionGpu.MeshletVertexTriangleIndexBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletVertexTriangleIndexBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescriptor.MeshletVertexTrianglesIndexCount * sizeof(_StaticMeshRenderData.MeshletVertexTrianglesIndex[0]))
            .SetName(Name + std::format("Meshlet Vertex Buffer LOD {}", LodIndex))
                .Build();

        const uint32_t * MesletVertexTriangleStart = _StaticMeshRenderData.MeshletVertexTrianglesIndex.data() + MeshLodDescriptor.MeshletVertexTrianglesIndexOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletVertexTriangleIndexBuffer.get(), MesletVertexTriangleStart, MeshSectionGpu.MeshletVertexTriangleIndexBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletTriangleBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletTriangleBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescriptor.MeshletTrianglesCount * sizeof(_StaticMeshRenderData.MeshletTriangles[0]))
            .SetName(Name + std::format("Meshlet Triangles Buffer LOD {}", LodIndex))
                .Build();

        const uint32_t * MeshletTriangleStart = _StaticMeshRenderData.MeshletTriangles.data() + MeshLodDescriptor.MeshletTrianglesOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletTriangleBuffer, MeshletTriangleStart, MeshSectionGpu.MeshletTriangleBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.MeshletBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(MeshLodDescriptor.MeshetCount * sizeof(_StaticMeshRenderData.Meshlets[0]))
            .SetName(Name + std::format("Meshlet Buffer LOD {}", LodIndex))
            .Build();

        const Meshlet* MeshletStart = _StaticMeshRenderData.Meshlets.data() + MeshLodDescriptor.MeshetOffset;
        _Branch->BufferUpload(*MeshSectionGpu.MeshletBuffer, MeshletStart, MeshSectionGpu.MeshletBuffer->GetSizeInByte());

        // Copy pos
        std::vector<Rendering::Gpu::vec4> position;
        position.resize(MeshLodDescriptor.VertexCount);
        for (size_t i = 0; i < position.size(); i++)
        {
            position[i].data[0] = _StaticMeshRenderData.Vertices[MeshLodDescriptor.VertexOffset + i].Position.x;
            position[i].data[1] = _StaticMeshRenderData.Vertices[MeshLodDescriptor.VertexOffset + i].Position.y;
            position[i].data[2] = _StaticMeshRenderData.Vertices[MeshLodDescriptor.VertexOffset + i].Position.z;
            position[i].data[3] = 1.f;
        }

        MeshSectionGpu.PositionBuffer.reset(rhi.CreateBuffer());
        MeshSectionGpu.PositionBuffer->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSizeInBytes(position.size() * sizeof(Rendering::Gpu::vec4))
            .SetName(Name + std::format("Meshlet Position Buffer LOD {}", LodIndex))
            .Build();


        _Branch->BufferUpload(*MeshSectionGpu.PositionBuffer, position.data(), MeshSectionGpu.PositionBuffer->GetSizeInByte());

        MeshSectionGpu.MeshletDescriptor.reset(rhi.CreateDescriptorSet());
        MeshSectionGpu.MeshletDescriptor
            ->BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 0, MeshSectionGpu.PositionBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 1, MeshSectionGpu.MeshletBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 2, MeshSectionGpu.MeshletVertexTriangleIndexBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 3, MeshSectionGpu.MeshletTriangleBuffer.get())
            .SetName(Name + std::format("Meshlet Bindings LOD {}", LodIndex))
            .Build();

        MeshSectionGpu.MeshLetCount = _StaticMeshRenderData.Meshlets.size();
    }

    if (MeshLodDescriptor.IndiceCount > 0 && MeshLodDescriptor.VertexCount > 0)
    {
        // VertexBuffer
        MeshSectionGpu.VertexBuffer = VertexBuffer(rhi);
        MeshSectionGpu.VertexBuffer
            .SetVerticiesCount(MeshLodDescriptor.IndiceCount)
            .SetVerticiesSize(sizeof(StaticMeshVertex))
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetSizeInBytes(MeshLodDescriptor.VertexCount * sizeof(StaticMeshVertex))
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex)
            .SetName(Name + " Vertex Buffer")
            .Build();


        MeshSectionGpu.IndexBuffer = IndexBuffer(rhi);
        MeshSectionGpu.IndexBuffer
            .SetIndexCount(MeshLodDescriptor.IndiceCount)
            .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetSizeInBytes(MeshLodDescriptor.IndiceCount * static_cast<size_t>(RhiBuffer::IndexFormat::Uint32))
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Index)
            .SetName(Name + " Index Buffer")
            .Build();


        const StaticMeshVertex* VerticiesStart = m_StaticMeshData.RenderData.Vertices.data() + MeshLodDescriptor.VertexOffset;
        const uint32_t* IndexBufferStart = m_StaticMeshData.RenderData.Indices.data() + MeshLodDescriptor.IndicesOffset;
        _Branch
            ->BufferUpload(*MeshSectionGpu.VertexBuffer.Get(), VerticiesStart, MeshSectionGpu.VertexBuffer->GetSizeInByte())
            .BufferUpload(*MeshSectionGpu.IndexBuffer.Get(), IndexBufferStart, MeshSectionGpu.IndexBuffer->GetSizeInByte());

    }
}

StaticMesh::StaticMesh() 
    : Resource()
{
    DYNAMIC_REFLECT_INIT
}
