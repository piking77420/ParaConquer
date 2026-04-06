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
	Tbx::Vector4f Position;
	Tbx::Vector4f Normal;
	Tbx::Vector4f Tangent;
	Tbx::Vector2f Uv;
	Tbx::Vector2f pad;

	static constexpr VertexInputBindingDescrition GetVertexBindingDescription(uint32_t _binding);

	static constexpr std::vector<VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding)
	{
		return
		{
			{
				.Binding = _binding,
				.Location = 0,
				.Format = RhiFormat::R32G32B32A32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Position)
			},
			{
				.Binding = _binding,
				.Location = 1,
				.Format = RhiFormat::R32G32B32A32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Normal)
			},
			{
				.Binding = _binding,
				.Location = 2,
				.Format = RhiFormat::R32G32B32A32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Tangent)
			},
			{
				.Binding = _binding,
				.Location = 3,
				.Format = RhiFormat::R32G32Sfloat,
				.Offset = offsetof(StaticMeshVertex, Uv)
			},
			{
				.Binding = _binding,
				.Location = 4,
				.Format = RhiFormat::R32G32Sfloat,
				.Offset = offsetof(StaticMeshVertex, pad)
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
	uint32_t IndicesCount;

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
// Currently each lod are separte in buffers  
struct MeshLOD
{
	std::vector<MeshSection> MeshesSections;
	MeshDataDescriptor Descriptor;

	size_t MeshletCount() const
	{
		size_t Count = 0ull;
		if (MeshesSections.empty())
			return Count;

		return MeshesSections[MeshesSections.size() - 1].MeshDataDescriptor.MeshetOffset + MeshesSections[MeshesSections.size() - 1].MeshDataDescriptor.MeshetCount;
	}
};

struct StaticMeshRenderData
{
	std::vector<StaticMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Meshlet>  Meshlets;
	std::vector<uint32_t> MeshletVertexTrianglesIndex;
	std::vector<uint32_t> MeshletTriangles;
	std::vector<MeshDataDescriptor> BaseMeshDescriptor;
	std::vector<MotionCore::Aabb<double>> MeshletAABB;
};

struct MeshDrawCommand
{
	uint32_t MeshSectionIndex;
};

struct StaticMeshData
{
	std::vector<PC_CORE::MeshDrawCommand> DrawCommands;
	std::vector<MeshLOD> MeshLods;
	MotionCore::Aabb<double> AABB;
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

	StaticMesh(const StaticMesh&) = delete;
	StaticMesh& operator=(const StaticMesh&) = delete;

	StaticMesh(StaticMesh&&) noexcept = default;
	StaticMesh& operator=(StaticMesh&&) noexcept = default;

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

	bool IsBuildForMeshlet() const
	{
		return m_IsBuildForMeshlet;
	}

	size_t GetMeshletCount(uint32_t LodIndex) const
	{
		return m_MeshSectionGpu[LodIndex].MeshLetCount > 0ull;
	}

	const StaticMeshData& GetStaticMeshData() const
	{
		return m_StaticMeshData;
	}

	RhiDescriptorSet* GetMeshletDescriptor(uint32_t LodIndex) const
	{
		return m_MeshSectionGpu[LodIndex].MeshletDescriptor.get();
	}

	const std::vector<double>& GetLodThreshold() const
	{
		return m_LODThreshold;
	}

	const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& GetBaseMaterial() const;

private:
		struct MeshSectionGpu
		{
			VertexBuffer VertexBuffer;

			IndexBuffer IndexBuffer;

			std::shared_ptr<RhiBuffer> MeshletBuffer;

			std::shared_ptr<RhiBuffer> MeshletAABBBuffer;

			std::shared_ptr<RhiBuffer> MeshletVertexTriangleIndexBuffer;

			std::shared_ptr<RhiBuffer> MeshletTriangleBuffer;

			std::shared_ptr<RhiDescriptorSet> MeshletDescriptor;

			size_t MeshLetCount{ 0 };
		};

	std::vector<MeshSectionGpu> m_MeshSectionGpu;

	StaticMeshData m_StaticMeshData;

	MotionCore::Aabb<double> m_Aabb;

	std::vector<double> m_LODThreshold;

	bool m_HallowCpuAcces = false;

	bool m_IsBuildForMeshlet = false;

	std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>> m_BaseMaterials;

	void InitFromRenderData(const StaticMeshData& _StaticMeshData, RHI::ResourceUpdateBranch* _Branch);

	void InitMeshSectionGpu(const StaticMeshData& _StaticMeshData, size_t LodIndex, RHI::ResourceUpdateBranch* _Branch);

	REFLECT(StaticMesh, Resource)
	REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
	REFLECT_MEMBER(StaticMesh, m_Aabb)
};

END_PCCORE
