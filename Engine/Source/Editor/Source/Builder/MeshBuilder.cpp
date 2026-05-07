#include "Builder/MeshBuilder.hpp"

#include <future>
#include <Thread/ThreadPool.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <meshoptimizer.h>

namespace PC_EDITOR_CORE
{
	MeshBuilder::MeshBuilderData MeshBuilder::BuildMeshs( 
		PC_CORE::Thread::ThreadPool& ThreadPool, 
		const aiScene* Scene, 
		bool Optimise)
	{
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        std::vector<PC_CORE::StaticMeshVertex> VertexData;
        std::vector<uint32_t> IndiciesData;
        std::vector<MeshDescriptor> MeshDescriptorsData;
        LoadMeshesFromAiScene(&VertexData, &IndiciesData, &MeshDescriptorsData, Scene);
  
       

        if (!Optimise)
        {
            return MeshBuilder::MeshBuilderData(std::move(VertexData), std::move(IndiciesData), std::move(MeshDescriptorsData));
        }

        // Enqueu Data
        std::vector<std::future<OutOptimiseBuild>> futures;
        futures.reserve(MeshDescriptorsData.size());

        for (size_t i = 0; i < MeshDescriptorsData.size(); i++)
        {
            const std::span<const PC_CORE::StaticMeshVertex> SpanV = std::span<const PC_CORE::StaticMeshVertex>(VertexData.data() + MeshDescriptorsData[i].VertexOffset, MeshDescriptorsData[i].VertexCount);
            const std::span<const uint32_t> SpanI = std::span<const uint32_t>(IndiciesData.data() + MeshDescriptorsData[i].IndicesOffset, MeshDescriptorsData[i].IndicesCount);
            futures.emplace_back(
                ThreadPool.Enqueue(
                    OptmiseMesh, 
                    SpanV,
                    SpanI
                )
            );
        }

        MeshBuilderData Optimised;
        Optimised.Verticies.reserve(VertexData.size());
        Optimised.Indicies.reserve(IndiciesData.size());
        MeshDescriptorsData.clear();

        for (size_t i = 0; i < futures.size(); i++)
        {
            auto& f = futures[i];
            OutOptimiseBuild Data = f.get();

            MeshDescriptorsData.emplace_back(MeshDescriptor
                {
                    .VertexOffset = static_cast<uint32_t>(Optimised.Verticies.size()),
                    .VertexCount = static_cast<uint32_t>(Data.OutVertices.size()),
                    .IndicesOffset = static_cast<uint32_t>(Optimised.Indicies.size()),
                    .IndicesCount = static_cast<uint32_t>(Data.OutIndices.size()),
                });
            Optimised.Verticies.append_range(Data.OutVertices);
            Optimised.Indicies.append_range(Data.OutIndices);
        }

        Optimised.MeshDescriptor = std::move(MeshDescriptorsData);
        return Optimised;
	}

    MeshBuilder::MeshletOutPutData MeshBuilder::BuildMeshlets(PC_CORE::Thread::ThreadPool& ThreadPool, const MeshBuilderData& MeshBuilderData)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);
        MeshBuilder::MeshletOutPutData MeshletOutPutData;
        BuildMeshletsBase(ThreadPool, MeshletOutPutData, MeshBuilderData);
       
        return MeshletOutPutData;
    }

    void MeshBuilder::BuildMeshletsBase(PC_CORE::Thread::ThreadPool& ThreadPool, MeshBuilder::MeshletOutPutData& MeshletOutPutData, const MeshBuilderData& MeshBuilderData)
    {
        std::vector<std::future<OutMeshletBuild>> Futures;

        MeshletOutPutData.MeshletDescriptor.reserve(MeshBuilderData.MeshDescriptor.size());
        Futures.reserve(MeshBuilderData.MeshDescriptor.size());

        for (auto& Descriptor : MeshBuilderData.MeshDescriptor)
        {
            std::span<const PC_CORE::StaticMeshVertex> spanV = std::span<const PC_CORE::StaticMeshVertex>(MeshBuilderData.Verticies.data() + Descriptor.VertexOffset, Descriptor.VertexCount);
            std::span<const uint32_t> spaI = std::span<const uint32_t>(MeshBuilderData.Indicies.data() + Descriptor.IndicesOffset, Descriptor.IndicesCount);

            Futures.emplace_back(
                ThreadPool.Enqueue(
                    BuildMeshelts,
                    spanV,
                    spaI,
                    MeshBuilderData
                )
            );
        }

        for (auto& f : Futures)
        {
            OutMeshletBuild Data = f.get();
            MeshletOutPutData.MeshletDescriptor.emplace_back(
                MeshletDescriptor {
                    .MeshletOffset = static_cast<uint32_t>(MeshletOutPutData.Meshlets.size()),
                    .MeshletCount = static_cast<uint32_t>(Data.Meshlets.size()),

                    .MeshletVertexTriangleIndexOffset = static_cast<uint32_t>(MeshletOutPutData.MeshletVertexTrianglesIndex.size()),
                    .MeshletVertexTriangleIndexCount = static_cast<uint32_t>(Data.MeshletVertexTrianglesIndex.size()),

                    .MeshletTrianglesOffset = static_cast<uint32_t>(MeshletOutPutData.MeshletTrianglesU32.size()),
                    .MeshletTrianglesCount = static_cast<uint32_t>(Data.MeshletTrianglesU32.size()),
                }
                );
            MeshletOutPutData.Meshlets.append_range(Data.Meshlets);
            MeshletOutPutData.MeshletVertexTrianglesIndex.append_range(Data.MeshletVertexTrianglesIndex);
            MeshletOutPutData.MeshletTrianglesU32.append_range(Data.MeshletTrianglesU32);
            MeshletOutPutData.MeshletsBound.append_range(Data.Bounds);
        }
    }

    MeshBuilder::OutOptimiseBuild MeshBuilder::OptmiseMesh(const std::span<const PC_CORE::StaticMeshVertex>& Verticies, const std::span<const uint32_t>& Indices)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);
#if 1 
        const size_t NumIndicies = Indices.size();
        const size_t NumVerticies = Verticies.size();
        constexpr size_t SizeOfVertex = sizeof(std::remove_cv_t<std::remove_reference_t<decltype(Verticies)>>::value_type);

        std::vector<uint32_t> Remap(NumVerticies);
        const size_t OptVerticesCount = meshopt_generateVertexRemap(
            Remap.data(),
            Indices.data(),
            NumIndicies,
            Verticies.data(),
            NumVerticies,
            SizeOfVertex);


        std::vector<PC_CORE::StaticMeshVertex> OptVerticies{};
        std::vector<uint32_t> OptIndicies{};
        OptVerticies.resize(OptVerticesCount);
        OptIndicies.resize(NumIndicies);

        // remove duplicate Indicies
        meshopt_remapIndexBuffer(OptIndicies.data(), Indices.data(), NumIndicies, Remap.data());
        meshopt_remapVertexBuffer(OptVerticies.data(), Verticies.data(), NumVerticies, SizeOfVertex, Remap.data());


        meshopt_optimizeVertexCache(OptIndicies.data(), OptIndicies.data(), NumIndicies, OptVerticesCount);

        meshopt_optimizeOverdraw(OptIndicies.data(), OptIndicies.data(), NumIndicies, &OptVerticies[0].Position.x, OptVerticesCount, SizeOfVertex, 1.05f);

        meshopt_optimizeVertexFetch(OptVerticies.data(), OptIndicies.data(), NumIndicies, OptVerticies.data(), OptVerticesCount, SizeOfVertex);

        return OutOptimiseBuild(OptVerticies, OptIndicies);
#else
        return OutOptimiseBuild(std::vector<PC_CORE::StaticMeshVertex>(Verticies.begin(), Verticies.end()), std::vector<uint32_t>(Indices.begin(), Indices.end()));
#endif
    }

    MeshBuilder::OutMeshletBuild MeshBuilder::BuildMeshelts(const std::span<const PC_CORE::StaticMeshVertex>& _Verticies, const std::span<const uint32_t>& _Indices, const MeshBuilderData& MeshBuilderData)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        static_assert(sizeof(meshopt_Meshlet) == sizeof(PC_CORE::Meshlet));

        std::vector<PC_CORE::Meshlet> MeshletsOpt;
        std::vector<uint32_t>   MeshletVertexTrianglesIndex;
        std::vector<uint8_t>    meshletTriangles;

        const size_t maxMeshlets = meshopt_buildMeshletsBound(_Indices.size(), PC_CORE::Meshlet::MeshletMaxVertices, PC_CORE::Meshlet::MeshletMaxTriangle);

        MeshletsOpt.resize(maxMeshlets);
        MeshletVertexTrianglesIndex.resize(maxMeshlets * PC_CORE::Meshlet::MeshletMaxVertices);
        meshletTriangles.resize(maxMeshlets * PC_CORE::Meshlet::MeshletMaxTriangle * 3);

        constexpr float kConeWeight = 0.0f;

        size_t meshletCount = 0ull;
        {
            PERF_REGION_SCOPED;
            PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "meshopt_buildMeshlets");
            meshletCount = meshopt_buildMeshlets(
                reinterpret_cast<meshopt_Meshlet*>(MeshletsOpt.data()),
                MeshletVertexTrianglesIndex.data(),
                meshletTriangles.data(),
                reinterpret_cast<const uint32_t*>(_Indices.data()),
                _Indices.size(),
                reinterpret_cast<const float*>(_Verticies.data()),
                _Verticies.size(),
                sizeof(PC_CORE::StaticMeshVertex),
                PC_CORE::Meshlet::MeshletMaxVertices,
                PC_CORE::Meshlet::MeshletMaxTriangle,
                kConeWeight);

        }
        // Finally, resize (or trim) the storage to fit the actual number of meshlets built.
        //  meshopt_buildMeshletsBound return the worst scnerio size
        // Shrink
        if (meshletCount == 0)
            return {};
        auto& last = MeshletsOpt[meshletCount - 1];
        MeshletVertexTrianglesIndex.resize(last.VertexOffset + last.VertexCount);
        meshletTriangles.resize(last.TriangleOffset + ((last.TriangleCount * 3 + 3) & ~3));
        MeshletsOpt.resize(meshletCount);


        std::vector<PC_CORE::MeshletBound> MeshletBounds;
        MeshletBounds.reserve(MeshletsOpt.size());
        {
            PERF_REGION_SCOPED;
            PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "meshopt_computeMeshletBounds");
            for (const auto& m : MeshletsOpt)
            {
                const auto Bounds = meshopt_computeMeshletBounds(
                    &MeshletVertexTrianglesIndex[m.VertexOffset],
                    &meshletTriangles[m.TriangleOffset],
                    m.TriangleCount,
                    &_Verticies[0].Position.x,
                    static_cast<uint32_t>(_Verticies.size()),
                    sizeof(PC_CORE::StaticMeshVertex)
                );

                MeshletBounds.emplace_back(Tbx::Vector3f(Bounds.center[0], Bounds.center[1], Bounds.center[2]), Bounds.radius);
            }
        }
        
        std::vector<uint32_t> MeshletTrianglesU32;

        {
            PERF_REGION_SCOPED;
            PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "Pack Meshlet Indicies");
            for (auto& m : MeshletsOpt) {
                // Save triangle offset for current meshlet
                uint32_t triangleOffset = static_cast<uint32_t>(MeshletTrianglesU32.size());

                // Repack to uint32_t
                for (uint32_t i = 0; i < m.TriangleCount; ++i) {
                    uint32_t i0 = 3 * i + 0 + m.TriangleOffset;
                    uint32_t i1 = 3 * i + 1 + m.TriangleOffset;
                    uint32_t i2 = 3 * i + 2 + m.TriangleOffset;

                    uint8_t  vIdx0 = meshletTriangles[i0];
                    uint8_t  vIdx1 = meshletTriangles[i1];
                    uint8_t  vIdx2 = meshletTriangles[i2];
                    const uint32_t packed = ((static_cast<uint32_t>(vIdx0) & 0xFF) << 0) |
                        ((static_cast<uint32_t>(vIdx1) & 0xFF) << 8) |
                        ((static_cast<uint32_t>(vIdx2) & 0xFF) << 16);
                    MeshletTrianglesU32.push_back(packed);
                }

                // Update triangle offset for current meshlet
                m.TriangleOffset = triangleOffset;
            }
        }
       

        OutMeshletBuild Out;
        Out.Meshlets = std::move(MeshletsOpt);
        Out.MeshletVertexTrianglesIndex = std::move(MeshletVertexTrianglesIndex);
        Out.MeshletTrianglesU32 = std::move(MeshletTrianglesU32);
        Out.Bounds = std::move(MeshletBounds);

        return Out;
    }

    void MeshBuilder::LoadMeshesFromAiScene(std::vector<PC_CORE::StaticMeshVertex>* _Verticies, std::vector<uint32_t>* _Indices, std::vector<MeshDescriptor>* _MeshDescriptors, const aiScene* Scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);


        assert(_Verticies != nullptr);
        assert(_Indices != nullptr);
        assert(_MeshDescriptors != nullptr);

        std::vector<PC_CORE::StaticMeshVertex>& VertexData = *_Verticies;
        std::vector<uint32_t>& IndiciesData = *_Indices;
        std::vector<MeshDescriptor>& MeshDescriptorsData = *_MeshDescriptors;

        uint32_t VertexCount = 0u;
        uint32_t IndexCount = 0u;

        uint32_t MeshOffsetAndCountCount = 0;
        MeshDescriptorsData.reserve(Scene->mNumMeshes);

        for (size_t i = 0; i < Scene->mNumMeshes; i++)
        {
            const aiMesh* Mesh = Scene->mMeshes[i];

            uint32_t MeshIndiciesCount = 0;
            for (size_t i = 0; i < Mesh->mNumFaces; i++)
                MeshIndiciesCount += Mesh->mFaces[i].mNumIndices;

            MeshDescriptorsData.emplace_back(MeshDescriptor
                {
                    .VertexOffset = VertexCount,
                    .VertexCount = Scene->mMeshes[i]->mNumVertices,
                    .IndicesOffset = IndexCount,
                    .IndicesCount = MeshIndiciesCount,
                });

            IndexCount += MeshIndiciesCount;
            VertexCount += Mesh->mNumVertices;
        }
        VertexData.reserve(VertexCount);
        IndiciesData.reserve(IndexCount);

        for (size_t i = 0; i < Scene->mNumMeshes; i++)
        {
            // Vertex
            const aiMesh* Mesh = Scene->mMeshes[i];
            FillVertices(VertexData, *Mesh);
            FillIndices(IndiciesData, *Mesh);
        }
    }

    void MeshBuilder::FillVertices(std::vector<PC_CORE::StaticMeshVertex>& _Verticies, const ::aiMesh& _Meshes)
    {
        for (size_t v = 0; v < _Meshes.mNumVertices; v++)
        {
            PC_CORE::StaticMeshVertex Vertex{};
            Vertex.Position = Tbx::Vector4f{ _Meshes.mVertices[v].x, _Meshes.mVertices[v].y, _Meshes.mVertices[v].z, 0.0f };

            if (_Meshes.HasNormals())
                Vertex.Normal = Tbx::Vector4f{ _Meshes.mNormals[v].x, _Meshes.mNormals[v].y, _Meshes.mNormals[v].z, 0.0f };

            if (_Meshes.HasTextureCoords(0))
                Vertex.Uv = Tbx::Vector2f{ _Meshes.mTextureCoords[0][v].x, _Meshes.mTextureCoords[0][v].y };

            if (_Meshes.HasTangentsAndBitangents())
                Vertex.Tangent = Tbx::Vector4f{ _Meshes.mTangents[v].x, _Meshes.mTangents[v].y, _Meshes.mTangents[v].z, 0.0f };

            
            _Verticies.emplace_back(std::move(Vertex));
        }
    }

    void MeshBuilder::FillIndices(std::vector<uint32_t>& _Indices, const ::aiMesh& _Meshes)
    {
        for (size_t i = 0; i < _Meshes.mNumFaces; i++)
        {
            for (size_t j = 0; j < _Meshes.mFaces[i].mNumIndices; j++)
                _Indices.emplace_back(_Meshes.mFaces[i].mIndices[j]);
        }
    }

}


