#include "Resources/StaticMesh.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiResource.hpp"
#include "LowRenderer/RhiResourceUpdate.hpp"
#include "CompactBuffer.hpp"

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

        
        m_VertexBuffer = VertexBuffer(Name + "VertexBuffer" ,verticiesRaw.size(), sizeof(StaticMeshVertex), PC_CORE::RhiResource::MemoryUsage::Static);
        m_VertexBuffer->Build();
        m_VertexBuffer->UploadData(verticiesRaw.data(), verticiesRaw.size() * sizeof(StaticMeshVertex));
        
        m_IndexBuffer = IndexBuffer(Name + "VertexBuffer" , indiciesRaw.size(), RhiBuffer::IndexFormat::Uint32 , PC_CORE::RhiResource::MemoryUsage::Static);
        m_IndexBuffer->Build();
        m_IndexBuffer->UploadData(indiciesRaw.data(), indiciesRaw.size() * sizeof(StaticMeshVertex));

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

StaticMesh::StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshRenderData(_StaticMeshRenderData)
{
    DYNAMIC_REFLECT_INIT

    InitFromRenderData(_StaticMeshRenderData, _Branch);

    if (!m_HallowCpuAcces)
    {
        m_StaticMeshRenderData.Vertices.clear();
        m_StaticMeshRenderData.Indices.clear();
    }

}

StaticMesh::StaticMesh(std::string _Name, StaticMeshRenderData&& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshRenderData(std::move(_StaticMeshRenderData))
{
    DYNAMIC_REFLECT_INIT

    InitFromRenderData(_StaticMeshRenderData, _Branch);

    if (!m_HallowCpuAcces)
    {
        m_StaticMeshRenderData.Vertices.clear();
        m_StaticMeshRenderData.Indices.clear();
    }

}

const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& StaticMesh::GetBaseMaterial() const
{
    return m_BaseMaterials;
}

void StaticMesh::InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
{
    Rhi& rhi = App::Instance->RenderHarwareInteface;

    if (!_StaticMeshRenderData.MeshletVertices.empty() || !_StaticMeshRenderData.MeshletTriangles.empty() || !_StaticMeshRenderData.Meshlets.empty())
    {
        assert(!_StaticMeshRenderData.MeshletVertices.empty() && !_StaticMeshRenderData.MeshletTriangles.empty() && !_StaticMeshRenderData.Meshlets.empty());

        m_MeshletVerticiesBuffer.reset(rhi.CreateBuffer());
        m_MeshletVerticiesBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSize(_StaticMeshRenderData.MeshletVertices.size() * sizeof(_StaticMeshRenderData.MeshletVertices[0]))
            .SetName(Name + "Meshlet Vertex Buffer")
            .Build();
        _Branch->BufferUpload(*m_MeshletVerticiesBuffer, _StaticMeshRenderData.MeshletVertices.data(), m_MeshletVerticiesBuffer->GetSize());

        m_MeshletTriangleBuffer.reset(rhi.CreateBuffer());
        m_MeshletTriangleBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSize(_StaticMeshRenderData.MeshletTriangles.size() * sizeof(_StaticMeshRenderData.MeshletTriangles[0]))
            .SetName(Name + "Meshlet Triangles Buffer")
            .Build();

        _Branch->BufferUpload(*m_MeshletTriangleBuffer, _StaticMeshRenderData.MeshletTriangles.data(), m_MeshletTriangleBuffer->GetSize());

        m_MeshletBuffer.reset(rhi.CreateBuffer());
        m_MeshletBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
            .SetSize(_StaticMeshRenderData.Meshlets.size() * sizeof(_StaticMeshRenderData.Meshlets[0]))
            .SetName(Name + "Meshlet Buffer")
            .Build();

        _Branch->BufferUpload(*m_MeshletBuffer, _StaticMeshRenderData.Meshlets.data(), m_MeshletBuffer->GetSize());


        m_MeshLetCount = _StaticMeshRenderData.Meshlets.size();
    }

    // VertexBuffer
    m_VertexBuffer = VertexBuffer(rhi);
    m_VertexBuffer
        .SetVerticiesCount(m_StaticMeshRenderData.Vertices.size())
        .SetVerticiesSize(sizeof(StaticMeshVertex))
        ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
        .SetSize(m_StaticMeshRenderData.Vertices.size() * sizeof(StaticMeshVertex))
        .SetUsage(HasMeshlet() ? RhiBuffer::BufferUsageFlagBits::Vertex : RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::ShaderStorage)
        .SetName(Name + " Vertex Buffer")
        .Build();


    m_IndexBuffer = IndexBuffer(rhi);
    m_IndexBuffer
        .SetIndexCount(m_StaticMeshRenderData.Indices.size())
        .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
        ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
        .SetSize(m_StaticMeshRenderData.Indices.size() * static_cast<size_t>(RhiBuffer::IndexFormat::Uint32))
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Index)
        .SetName(Name + " Index Buffer")
        .Build();

    _Branch
        ->BufferUpload(*m_VertexBuffer.Get(), m_StaticMeshRenderData.Vertices.data(), m_VertexBuffer->GetSize())
        .BufferUpload(*m_IndexBuffer.Get(), m_StaticMeshRenderData.Indices.data(), m_IndexBuffer->GetSize());

    if (HasMeshlet())
    {
        m_MeshletDescriptor.reset(rhi.CreateDescriptorSet());
        m_MeshletDescriptor
            ->BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 0, m_VertexBuffer.Get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 1, m_MeshletBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 2, m_MeshletVerticiesBuffer.get())
            .BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 3, m_MeshletTriangleBuffer.get())
            .SetName(Name + " Meshlet Bindings")
            .Build();
    }
    
}



StaticMesh::StaticMesh() 
    : Resource()
{
    DYNAMIC_REFLECT_INIT
}
