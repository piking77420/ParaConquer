#pragma once
#include "Mesh.hpp"
#include "CompactBuffer.hpp"

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
	REFLECT_MEMBER(StaticMeshVertex, position)
	REFLECT_MEMBER(StaticMeshVertex, normal)
	REFLECT_MEMBER(StaticMeshVertex, uv)
	REFLECT_MEMBER(StaticMeshVertex, tangent)

};



struct StaticMeshRenderData
{
	std::vector<StaticMeshVertex> vertices;  
	std::vector<uint32_t> indices;
};


struct StaticMeshCreateInfo
{
	std::string name;
	PC_CORE::StaticMeshRenderData staticMeshRenderData;
	bool hallowCpuAcces;
};




class PC_CORE_API StaticMesh : public Resource
{
public:
	VertexBuffer vertexBuffer;

	IndexBuffer indexBuffer;

	void AfterSerialize(Serializer* serializer) const override;

	void AfterDeSerialize(Serializer* serializer) override ;

	const MotionCore::Aabb<double>& GetAABB() const
	{
		return m_Aabb;
	}

	bool GetAlloWCpuAcces() const
	{
		return m_HallowCpuAcces;
	}

	IMP_DYNAMIC_REFLECT()

	DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

	StaticMesh(const StaticMeshCreateInfo& _staticMeshCreateInfo);
	
	StaticMesh();

	virtual ~StaticMesh() override  = default;

private:
	bool m_HallowCpuAcces = false;
	
	MotionCore::Aabb<double> m_Aabb;

	StaticMeshRenderData m_RenderData;

	REFLECT(StaticMesh, Resource)
	REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
	REFLECT_MEMBER(StaticMesh, m_Aabb)
};

END_PCCORE