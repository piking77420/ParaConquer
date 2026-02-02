#pragma once
#include "Resource.hpp"
#include "ObjectPtr.hpp"
#include "Mesh.hpp"
#include "Rendering/Material.hpp"

namespace PC_CORE::RHI
{
	class ResourceUpdateBranch;
}

BEGIN_PCCORE

struct Meshlet
{
	// from https://chaoticbob.github.io/2024/01/24/mesh-shading-part-1.html
	static constexpr size_t MeshletMaxTriangle = 124;
	static constexpr size_t MeshletMaxVertices = 64;

	// Data not in regular vertex and index buffer
	uint32_t VertexOffset;
	uint32_t TriangleOffset;

	uint32_t VertexCount;
	uint32_t TriangleCount;
};

struct StaticMeshVertex
{
	Tbx::Vector3f Position;
	Tbx::Vector3f Normal;
	Tbx::Vector3f Tangent;
	Tbx::Vector2f Uv;

	static constexpr VertexInputBindingDescrition GetVertexBindingDescription(uint32_t _binding);

	static constexpr std::vector<VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding)
	{
		return
		{
			{
				.Binding = _binding,
				.Location = 0,
				.Format = RhiFormat::R32G32B32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Position)
			},
			{
				.Binding = _binding,
				.Location = 1,
				.Format = RhiFormat::R32G32B32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Normal)
			},
			{
				.Binding = _binding,
				.Location = 2,
				.Format = RhiFormat::R32G32B32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Tangent)
			},
			{
				.Binding = _binding,
				.Location = 3,
				.Format = RhiFormat::R32G32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Uv)
			}


		};
	}


	REFLECT(StaticMeshVertex)
		REFLECT_MEMBER(StaticMeshVertex, Position)
		REFLECT_MEMBER(StaticMeshVertex, Normal)
		REFLECT_MEMBER(StaticMeshVertex, Uv)
		REFLECT_MEMBER(StaticMeshVertex, Tangent)


};

constexpr VertexInputBindingDescrition StaticMeshVertex::GetVertexBindingDescription(const uint32_t _binding)
{
	return
	{
		.Binding = _binding,
		.Stride = sizeof(StaticMeshVertex),
		.VertexInputRate = VertexInputRate::Vertex
	};
}

struct OffsetAndCount { size_t Offset = 0; size_t Count = 0; };

struct MeshLOD
{
	OffsetAndCount VertexSection;
	OffsetAndCount IndicesSection;
	OffsetAndCount MeshletsSection;
	OffsetAndCount MeshletsTriangleVertexIndexSection;
	OffsetAndCount MeshletsTrianglesSection;
	MotionCore::Aabb<double> AABB;
};

struct MeshSection
{
	std::string Name;
	std::vector<MeshLOD> LODs;

	// Section Offset To That mesh section 
	// Global Offset GPU -> begin submesh Load Sections
	OffsetAndCount VerticesGlobal;
	OffsetAndCount IndicesGlobal;
	OffsetAndCount MeshletsGlobal;
	OffsetAndCount MeshletsTriangleVertexIndexGlobal;
	OffsetAndCount MeshletsTrianglesGlobal;

	uint32_t MaterialIndex;
};

struct StaticMeshRenderData
{
	std::vector<StaticMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Meshlet>  Meshlets;
	std::vector<uint32_t> MeshletVertexTrianglesIndex;
	std::vector<uint32_t> MeshletTriangles;
};

struct MeshNode
{
	size_t MeshSectionIndex;
	Tbx::Matrix4x4d Transform;
};

// LOD are store Seqencally in gpu memory
	// SubMesh0_LOD0
	// SubMesh0_LOD1
	// SubMesh1_LOD0
	// SubMesh1_LOD1
struct StaticMeshData
{
	std::vector<MeshNode> StaticMeshNode; // TODO FILL
	std::vector<MeshSection> MeshSections;
	StaticMeshRenderData RenderData;
};
	
class PC_CORE_API StaticMesh : public Resource
{
public:

	explicit StaticMesh(std::string _Name, const PC_CORE::ObjectPtr<Resource>& SharedMesh, MeshSection subMesh);

	explicit StaticMesh(std::string _Name, const StaticMeshData& _StaticMeshData, RHI::ResourceUpdateBranch* _branch);

	explicit StaticMesh(std::string _Name, StaticMeshData&& _StaticMeshData, RHI::ResourceUpdateBranch* _branch);

	StaticMesh();

	~StaticMesh() override = default;

	DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

		IMP_DYNAMIC_REFLECT()

		void AfterSerialize(Serializer* _serializer) const override;

	void AfterDeSerialize(Serializer* _serializer) override;

	StaticMesh& SetAABB(const MotionCore::Aabb<double>& _AABB)
	{
		m_Aabb = _AABB;
		return *this;
	}

	StaticMesh& SetBaseMaterials(const std::vector<ObjectPtr<Rendering::Material>>& _Material);


	const VertexBuffer& GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	const IndexBuffer& GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	const MotionCore::Aabb<double>& GetAabb() const
	{
		return m_Aabb;
	}

	bool GetAlloWCpuAcces() const
	{
		return m_HallowCpuAcces;
	}

	bool HasMeshlet() const
	{
		return m_MeshLetCount > 0ull;
	}

	size_t GetMeshletCount() const
	{
		return m_MeshLetCount;
	}

	const StaticMeshData& GetStaticMeshData() const
	{
		return m_StaticMeshData;
	}

	RhiDescriptorSet* GetMeshletDescriptor() const
	{
		return m_MeshletDescriptor.get();
	}

	const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& GetBaseMaterial() const;

private:
	VertexBuffer m_VertexBuffer;

	IndexBuffer m_IndexBuffer;

	std::unique_ptr<RhiBuffer> m_MeshletVerticiesBuffer;

	std::unique_ptr<RhiBuffer> m_MeshletTriangleBuffer;

	std::unique_ptr<RhiBuffer> m_MeshletBuffer;

	std::unique_ptr<RhiBuffer> m_PositionBuffer;

	std::unique_ptr<RhiDescriptorSet> m_MeshletDescriptor;

	StaticMeshData m_StaticMeshData;

	MotionCore::Aabb<double> m_Aabb;

	size_t m_MeshLetCount{ 0 };

	bool m_HallowCpuAcces = false;

	std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>> m_BaseMaterials;

	void InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch);

	REFLECT(StaticMesh, Resource)
		REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
		REFLECT_MEMBER(StaticMesh, m_Aabb)
};

END_PCCORE
