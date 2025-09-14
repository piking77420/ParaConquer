#include "Resources/StaticMesh.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

void StaticMesh::AfterSerialize(Serializer* serializer) const
{
	PC_LOG("AfterSerialize Static Mesh")


	
}

void StaticMesh::AfterDeSerialize(Serializer* serializer)
{
	PC_LOG("AfterDeSerialize Static Mesh")
	
	StaticMeshRenderData renderData;
	serializer->DeSerialize(&renderData);
	if (renderData.indices.empty() || renderData.vertices.empty())
	{
		vertexBuffer = VertexBuffer( renderData.vertices.data(),  renderData.vertices.size(), sizeof(StaticMeshVertex), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static),
		indexBuffer = IndexBuffer(renderData.indices.data(), renderData.indices.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);

		if (m_HallowCpuAcces)
			m_RenderData = std::move(renderData);
	}
	

}

StaticMesh::StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo) : Resource(_staticMeshCreateInfo.name),
m_HallowCpuAcces(_staticMeshCreateInfo.hallowCpuAcces)

{
	DYNAMIC_REFLECT_INIT
	// TODO 
	// Compute AABB from verticies
	// set name

	const std::vector<StaticMeshVertex>& vertices = _staticMeshCreateInfo.staticMeshRenderData.vertices;
	const std::vector<uint32_t>& indicies = _staticMeshCreateInfo.staticMeshRenderData.indices;

	vertexBuffer = VertexBuffer(vertices.data(), vertices.size(), sizeof(StaticMeshVertex), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static),
	indexBuffer = IndexBuffer(indicies.data(), indicies.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);

	if (m_HallowCpuAcces)
	{
		m_RenderData = _staticMeshCreateInfo.staticMeshRenderData;
	}
}


StaticMesh::StaticMesh() : Resource()
{
	DYNAMIC_REFLECT_INIT
}