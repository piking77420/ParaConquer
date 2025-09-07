#include "Resources/StaticMesh.hpp"

using namespace PC_CORE;

StaticMesh::StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo) : Resource(_staticMeshCreateInfo.name) ,
m_Verticies(_staticMeshCreateInfo.verticies), m_Indicies(_staticMeshCreateInfo.indicies)
{
	DYNAMIC_REFLECT_INIT
	// TODO 
	// Compute AABB from verticies

	
}

StaticMesh::StaticMesh(StaticMeshCreateInfo&& _staticMeshCreateInfo) : Resource(std::move(_staticMeshCreateInfo.name)),
m_Verticies(std::move(_staticMeshCreateInfo.verticies)), m_Indicies(std::move(_staticMeshCreateInfo.indicies))
{
	DYNAMIC_REFLECT_INIT
	// TODO 
	// Compute AABB from verticies


}
