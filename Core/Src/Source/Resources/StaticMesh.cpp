#include "Resources/StaticMesh.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

void StaticMesh::AfterSerialize() const
{
	PC_LOG("AfterSerialize Static Mesh")

}

void StaticMesh::AfterDeSerialize()
{
	PC_LOG("AfterDeSerialize Static Mesh")

		if (!m_Verticies.empty() && !m_Indicies.empty())
		{
			vertexBuffer = VertexBuffer(m_Verticies.data(), m_Verticies.size(), sizeof(StaticMeshVertex), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);
			indexBuffer = IndexBuffer(m_Indicies.data(), m_Indicies.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);
		}
	
}

StaticMesh::StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo) : Resource(_staticMeshCreateInfo.name),
m_HallowCpuAcces(_staticMeshCreateInfo.hallowCpuAcces),
m_Verticies(_staticMeshCreateInfo.verticies), 
m_Indicies(_staticMeshCreateInfo.indicies),
vertexBuffer(m_Verticies.data(), m_Verticies.size(), sizeof(StaticMeshVertex), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static),
indexBuffer(m_Indicies.data(), m_Indicies.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static)
{
	DYNAMIC_REFLECT_INIT
	// TODO 
	// Compute AABB from verticies
	// set name
	
}

StaticMesh::StaticMesh(StaticMeshCreateInfo&& _staticMeshCreateInfo) : Resource(std::move(_staticMeshCreateInfo.name)),
m_HallowCpuAcces(_staticMeshCreateInfo.hallowCpuAcces),
m_Verticies(std::move(_staticMeshCreateInfo.verticies)), 
m_Indicies(std::move(_staticMeshCreateInfo.indicies)),
vertexBuffer(m_Verticies.data(), m_Verticies.size(), sizeof(StaticMeshVertex), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static),
indexBuffer(m_Indicies.data(), m_Indicies.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static)
{
	DYNAMIC_REFLECT_INIT
	// TODO 
	// Compute AABB from verticies
	// set name
}

StaticMesh::StaticMesh() : Resource()
{
	DYNAMIC_REFLECT_INIT
}