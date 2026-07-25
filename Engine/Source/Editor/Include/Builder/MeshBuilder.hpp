#ifndef EDITOR_MESH_BUILDER
#define EDITOR_MESH_BUILDER

#include <vector>

#include "EditorHeader.hpp"
#include "Resources/StaticMesh.hpp"

namespace PC_CORE::Thread
{
	class ThreadPool;
}

struct aiScene;
struct aiTexture;
struct aiString;
struct aiNode;
struct aiMesh;

namespace PC_EDITOR_CORE
{
	class MeshBuilder
	{
	public:
		struct MeshDescriptor
		{
			uint32_t VertexOffset;
			uint32_t VertexCount;
			uint32_t IndicesOffset;
			uint32_t IndicesCount;
		};

		struct MeshBuilderData
		{
			std::vector<PC_CORE::StaticMeshVertex> Verticies;
			std::vector<uint32_t> Indicies;
			std::vector<MeshDescriptor> MeshDescriptor;
		};

		struct MeshletDescriptor
		{
			uint32_t MeshletOffset;
			uint32_t MeshletCount;

			uint32_t MeshletVertexTriangleIndexOffset;
			uint32_t MeshletVertexTriangleIndexCount;

			uint32_t MeshletTrianglesOffset;
			uint32_t MeshletTrianglesCount;
		};

		struct MeshletOutPutData
		{
			std::vector<PC_CORE::Meshlet> Meshlets;
			std::vector<uint32_t> MeshletVertexTrianglesIndex;
			std::vector<uint32_t> MeshletTrianglesU32;

			std::vector<MeshletDescriptor> MeshletDescriptor;
			std::vector<PC_CORE::MeshletBound> MeshletsBound;
		};

		[[maybe_unused]] MeshBuilderData BuildMeshs(PC_CORE::Thread::ThreadPool& ThreadPool, const aiScene* Scene, bool Optimise = true);

		[[maybe_unused]] MeshletOutPutData BuildMeshlets(PC_CORE::Thread::ThreadPool& ThreadPool, const MeshBuilderData& MeshBuilderData);

		MeshBuilder() = default;

		~MeshBuilder() = default;
	private:
		MeshBuilderData m_BuilderData;

		struct OutOptimiseBuild
		{
			std::vector<PC_CORE::StaticMeshVertex> OutVertices;
			std::vector<uint32_t> OutIndices;
		};

		struct OutMeshletBuild
		{
			std::vector<PC_CORE::Meshlet> Meshlets;
			std::vector<uint32_t> MeshletVertexTrianglesIndex;
			std::vector<uint32_t> MeshletTrianglesU32;
			std::vector<PC_CORE::MeshletBound> Bounds;
		};

		static OutOptimiseBuild OptmiseMesh(const std::span<const PC_CORE::StaticMeshVertex>& Verticies, const std::span<const uint32_t>& Indices);

		static OutMeshletBuild BuildMeshelts(const std::span<const PC_CORE::StaticMeshVertex>& _Verticies, const std::span<const uint32_t>& _Indices, const MeshBuilderData& MeshBuilderData);

		void BuildMeshletsBase(PC_CORE::Thread::ThreadPool& ThreadPool, MeshBuilder::MeshletOutPutData &MeshletOutPutData, const MeshBuilderData& MeshBuilderData);

		void LoadMeshesFromAiScene(std::vector<PC_CORE::StaticMeshVertex>* _Verticies, std::vector<uint32_t>* _Indices, std::vector<MeshDescriptor>* _MeshDescriptors, const aiScene* Scene);

		void FillVertices(std::vector<PC_CORE::StaticMeshVertex>& _Verticies, const ::aiMesh& _Meshes);

		void FillIndices(std::vector<uint32_t>& _Indices, const ::aiMesh& _Meshes);
	};
}

#endif