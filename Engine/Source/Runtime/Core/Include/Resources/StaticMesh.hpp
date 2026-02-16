#pragma once
#include <optional>

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

struct MeshDataDescriptor 
{
	// Vertex
	uint32_t VertexOffset;
	uint32_t VertexCount;
	// Indicies
	uint32_t IndicesOffset;
	uint32_t IndiceCount;

	// Meshlets
	uint32_t MeshetOffset;
	uint32_t MeshetCount;

	// MeshletTrianglesIndexOffset
	uint32_t MeshletVertexTrianglesIndexOffset;
	uint32_t MeshletVertexTrianglesIndexCount;

	// MeshletTriangles
	uint32_t MeshletTrianglesOffset;
	uint32_t MeshletTrianglesCount;
};


struct MeshSection
{
	MotionCore::Aabb<double> AABB;

	MeshDataDescriptor MeshDataDescriptor;

	uint32_t MaterialIndex;
};

// LOD Strategie
// Currently each lod are separte into  

struct MeshLOD
{
	std::vector<MeshSection> MeshesSections;
};

struct StaticMeshRenderData
{
	std::vector<StaticMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Meshlet>  Meshlets;
	std::vector<uint32_t> MeshletVertexTrianglesIndex;
	std::vector<uint32_t> MeshletTriangles;

	MotionCore::Aabb<double> AABB;

	std::vector<MeshDataDescriptor> LODSDescriptor;// per lods
};

struct MeshDrawCommand
{
	Tbx::Matrix4x4d MeshSectionTransformL;
	uint32_t MeshSectionIndex;
};

struct StaticMeshData
{
	std::vector<MeshDrawCommand> MeshDrawCommands;
	std::vector<MeshLOD> meshLods;
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

	const VertexBuffer& GetVertexBuffer(uint32_t LodIndex) const
	{
		assert(LodIndex < m_MeshSectionGpu.size());

		return m_MeshSectionGpu[LodIndex].VertexBuffer;
	}

	const IndexBuffer& GetIndexBuffer(uint32_t LodIndex) const
	{
		assert(LodIndex < m_MeshSectionGpu.size());

		return m_MeshSectionGpu[LodIndex].IndexBuffer;
	}

	const MotionCore::Aabb<double>& GetAabb() const
	{
		return m_Aabb;
	}

	bool GetAlloWCpuAcces() const
	{
		return m_HallowCpuAcces;
	}

	bool IsBuildForMeshlet(uint32_t LodIndex) const
	{
		return m_MeshSectionGpu[LodIndex].MeshLetCount > 0ull;
	}

	size_t GetMeshletCount() const
	{
		return m_IsBuildForMeshlet;
	}

	const StaticMeshData& GetStaticMeshData() const
	{
		return m_StaticMeshData;
	}

	RhiDescriptorSet* GetMeshletDescriptor(uint32_t LodIndex) const
	{
		return m_MeshSectionGpu[LodIndex].MeshletDescriptor.get();
	}

	const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& GetBaseMaterial() const;

private:
	struct MeshSectionGpu
	{
		VertexBuffer VertexBuffer;

		IndexBuffer IndexBuffer;

		std::unique_ptr<RhiBuffer> MeshletVertexTriangleIndexBuffer;

		std::unique_ptr<RhiBuffer> MeshletTriangleBuffer;

		std::unique_ptr<RhiBuffer> MeshletBuffer;

		std::unique_ptr<RhiBuffer> PositionBuffer;

		std::unique_ptr<RhiDescriptorSet> MeshletDescriptor;

		size_t MeshLetCount{ 0 };
	};

	std::vector<MeshSectionGpu> m_MeshSectionGpu;

	StaticMeshData m_StaticMeshData;

	MotionCore::Aabb<double> m_Aabb;

	bool m_HallowCpuAcces = false;

	bool m_IsBuildForMeshlet = false;

	std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>> m_BaseMaterials;

	void InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch);

	void InitMeshSectionGpu(const StaticMeshRenderData& _StaticMeshRenderData, size_t LodIndex, RHI::ResourceUpdateBranch* _Branch);

	REFLECT(StaticMesh, Resource)
	REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
	REFLECT_MEMBER(StaticMesh, m_Aabb)
};

END_PCCORE
