#include "Resources/StaticMesh.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiResource.hpp"

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

        
        VBuffer = VertexBuffer(Name + "VertexBuffer" ,verticiesRaw.size(), sizeof(StaticMeshVertex), PC_CORE::RhiResource::MemoryUsage::Static);
        VBuffer->Build();
        VBuffer->UploadData(verticiesRaw.data(), verticiesRaw.size() * sizeof(StaticMeshVertex));
        
        IBuffer = IndexBuffer(Name + "VertexBuffer" , indiciesRaw.size(), RhiBuffer::IndexFormat::Uint32 , PC_CORE::RhiResource::MemoryUsage::Static);
        IBuffer->Build();
        IBuffer->UploadData(indiciesRaw.data(), indiciesRaw.size() * sizeof(StaticMeshVertex));

        if (m_HallowCpuAcces)
        {
            m_RenderData.Vertices = std::move(verticiesRaw);
            m_RenderData.Indices = std::move(indiciesRaw);
        }
    }*/
}

StaticMesh::StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData)
    : Resource(std::move(_Name))
{
    DYNAMIC_REFLECT_INIT

    Rhi& rhi = App::Instance->RenderHarwareInteface;

    // VertexBuffer
    VBuffer = VertexBuffer(rhi);
    VBuffer
        .SetVerticiesCount(_StaticMeshRenderData.Vertices.size())
        .SetVerticiesSize(sizeof(StaticMeshVertex))
        ->SetMemoryUsage(RhiMemoryUsage::Static)
        .SetSize(_StaticMeshRenderData.Vertices.size() * sizeof(StaticMeshVertex))
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex)
        .SetName(Name + " Vertex Buffer")
        .Build();


    IBuffer = IndexBuffer(rhi);
    IBuffer
        .SetIndexCount(_StaticMeshRenderData.Indices.size())
        .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
        ->SetMemoryUsage(RhiMemoryUsage::Static)
        .SetSize(_StaticMeshRenderData.Indices.size() * static_cast<size_t>(RhiBuffer::IndexFormat::Uint32))
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Index)
        .SetName(Name + " Index Buffer")
        .Build();


    rhi.GetRhiContext().ResourceUpdateBranch()
        ->BufferUpload(*VBuffer.Get(), _StaticMeshRenderData.Vertices.data(), VBuffer->GetSize())
        .BufferUpload(*IBuffer.Get(), _StaticMeshRenderData.Indices.data(), IBuffer->GetSize());

}


StaticMesh::StaticMesh() 
    : Resource()
{
    DYNAMIC_REFLECT_INIT
}
