#include "Resources/StaticMesh.hpp"

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

StaticMesh::StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo) 
    : Resource(_staticMeshCreateInfo.Name),
     m_HallowCpuAcces(_staticMeshCreateInfo.HallowCpuAcces)

{
    DYNAMIC_REFLECT_INIT
    // TODO 
    // Compute AABB from verticies
    // set name
        /*
    const std::vector<StaticMeshVertex>& vertices = _staticMeshCreateInfo.StaticMeshRenderData.Vertices;
    const std::vector<uint32_t>& indicies = _staticMeshCreateInfo.StaticMeshRenderData.Indices;

    VBuffer = VertexBuffer(Name + "VertexBuffer" ,vertices.size(), sizeof(StaticMeshVertex), PC_CORE::RhiResource::MemoryUsage::Static);
    VBuffer->Build();

    IBuffer = IndexBuffer(Name + "VertexBuffer" , indicies.size(), RhiBuffer::IndexFormat::Uint32 , PC_CORE::RhiResource::MemoryUsage::Static);
    IBuffer->Build();

    PC_CORE::Rhi::GetInstance().PushResourceUpdate([&](CommandList* _List)
        {
            VBuffer->UploadData(_List, vertices.data(), vertices.size() * sizeof(StaticMeshVertex));
            IBuffer->UploadData(_List, indicies.data(), indicies.size() * sizeof(StaticMeshVertex));
        });

    if (m_HallowCpuAcces)
    {
        m_RenderData = _staticMeshCreateInfo.StaticMeshRenderData;
    }*/
}


StaticMesh::StaticMesh() : Resource()
{
    DYNAMIC_REFLECT_INIT
}
