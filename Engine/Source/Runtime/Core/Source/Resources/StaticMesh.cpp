#include "Resources/StaticMesh.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiResource.hpp"
#include "LowRenderer/RhiResourceUpdate.hpp"
#include "CompactBuffer.hpp"

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


StaticMesh::StaticMesh(std::string _Name, const PC_CORE::ObjectPtr<Resource>& SharedMesh, PC_CORE::SubMesh subMesh)
    : Resource(std::move(_Name))
    , m_SharedMesh(SharedMesh)
    , m_SubMesh(subMesh)
{
    DYNAMIC_REFLECT_INIT
}

StaticMesh::StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
    : Resource(std::move(_Name))
    , m_StaticMeshRenderData(_StaticMeshRenderData)
    , m_IsSharedMesh(true)
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
    , m_IsSharedMesh(true)
{
    DYNAMIC_REFLECT_INIT

        InitFromRenderData(_StaticMeshRenderData, _Branch);

    if (!m_HallowCpuAcces)
    {
        m_StaticMeshRenderData.Vertices.clear();
        m_StaticMeshRenderData.Indices.clear();
    }

}



void StaticMesh::InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch)
{
    Rhi& rhi = App::Instance->RenderHarwareInteface;

    // VertexBuffer
    m_VertexBuffer = VertexBuffer(rhi);
    m_VertexBuffer
        .SetVerticiesCount(m_StaticMeshRenderData.Vertices.size())
        .SetVerticiesSize(sizeof(StaticMeshVertex))
        ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
        .SetSize(m_StaticMeshRenderData.Vertices.size() * sizeof(StaticMeshVertex))
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex)
        .SetName(Name + " Vertex Buffer")
        .Build();


    m_IndexBuffer = IndexBuffer(rhi);
    m_IndexBuffer
        .SetIndexCount(m_StaticMeshRenderData.Indices.size())
        .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
        ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
        .SetSize(m_StaticMeshRenderData.Indices.size()* static_cast<size_t>(RhiBuffer::IndexFormat::Uint32))
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Index)
        .SetName(Name + " Index Buffer")
        .Build();

    
    _Branch
        ->BufferUpload(*m_VertexBuffer.Get(), m_StaticMeshRenderData.Vertices.data(), m_VertexBuffer->GetSize())
        .BufferUpload(*m_IndexBuffer.Get(), m_StaticMeshRenderData.Indices.data(), m_IndexBuffer->GetSize());
    
}



StaticMesh::StaticMesh() 
    : Resource()
{
    DYNAMIC_REFLECT_INIT
}
