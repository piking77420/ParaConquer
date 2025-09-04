#pragma once
#include "Mesh.hpp"

BEGIN_PCCORE

struct StaticMeshVertex
{
	Tbx::Vector3f position;
	Tbx::Vector3f normal;
	Tbx::Vector2f uv;
	Tbx::Vector3f tangent;

	static constexpr PC_CORE::VertexInputBindingDescrition GetVertexBindingDescription(uint32_t _binding)
	{
		return
		{
			.binding = _binding,
			.stride = sizeof(StaticMeshVertex),
			.vertexInputRate = VertexInputRate::VERTEX
		};
	}

	static constexpr std::vector<PC_CORE::VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding)
	{
		return
		{
			{
				.binding = _binding,
				.location = 0,
				.format = RHIFormat::R32G32B32_SFLOAT,
				.offset = offsetof(StaticMeshVertex, position)
			},
			{
				.binding = _binding,
				.location = 1,
				.format = RHIFormat::R32G32B32_SFLOAT,
				.offset = offsetof(StaticMeshVertex, normal)
			},
			{
				.binding = _binding,
				.location = 2,
				.format = RHIFormat::R32G32_SFLOAT,
				.offset = offsetof(StaticMeshVertex, uv)
			},
			{
				.binding = _binding,
				.location = 3,
				.format = RHIFormat::R32G32B32_SFLOAT,
				.offset = offsetof(StaticMeshVertex, tangent)
			}


		};
	}


	REFLECT(StaticMeshVertex)
};

struct StaticMeshCreateInfo
{
	std::string name;
	std::vector<StaticMeshVertex> verticies;
	std::vector<uint32_t> indicies;
};

class PC_CORE_API StaticMesh : public Resource
{
public:
	VertexBuffer vertexBuffer;

	IndexBuffer indexBuffer;

	const std::vector<StaticMeshVertex>& GetVerticies() const
	{
		return m_Verticies;
	}

	const std::vector<uint32_t>& GetIndicies() const
	{
		return m_Indicies;
	}

	const MotionCore::Aabb<double>& GetAABB() const
	{
		return m_Aabb;
	}

	IMP_DYNAMIC_REFLECT()

	DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

	StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo);

	StaticMesh(StaticMeshCreateInfo&& _staticMeshCreateInfo);

	StaticMesh() 
	{
		DYNAMIC_REFLECT_INIT
	}

	~StaticMesh() override  = default;

private:
	std::vector<StaticMeshVertex> m_Verticies;

	std::vector<uint32_t> m_Indicies;

	MotionCore::Aabb<double> m_Aabb;


	REFLECT(StaticMesh, Resource)
	REFLECT_MEMBER(StaticMesh, m_Verticies)
	REFLECT_MEMBER(StaticMesh, m_Indicies)
	REFLECT_MEMBER(StaticMesh, m_Aabb)
};

END_PCCORE