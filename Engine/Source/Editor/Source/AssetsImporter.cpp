#include "AssetsImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>
#include <string_view>

#include "Rendering/Material.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Resources/FileLoader.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/StaticMesh.hpp"
#include "Serialize/Serializer.h"
#include "Thread/ThreadPool.hpp"

#include "meshoptimizer.h"

static inline std::string_view AssimpTextureTypeToString(aiTextureType aiTextureType)
{
    switch (aiTextureType)
    {
    case aiTextureType_NONE:
        break;
    case aiTextureType_DIFFUSE:
        return "Diffuse"sv;
    case aiTextureType_SPECULAR:
        return "Specular"sv;
    case aiTextureType_AMBIENT:
        return "Ambient"sv;
    case aiTextureType_EMISSIVE:
        return "Ambient"sv;
    case aiTextureType_HEIGHT:
        return "Height"sv;
    case aiTextureType_NORMALS:
        return "Normals"sv;
    case aiTextureType_SHININESS:
        return "Shininess"sv;
    case aiTextureType_OPACITY:
        return "Opacity"sv;
    case aiTextureType_DISPLACEMENT:
        return "Displacement"sv;
    case aiTextureType_LIGHTMAP:
        return "LightMap"sv;
    case aiTextureType_REFLECTION:
        return "Reflection"sv;
    case aiTextureType_BASE_COLOR:
        return "Ambient"sv;
    case aiTextureType_NORMAL_CAMERA:
        return "Color"sv;
    case aiTextureType_EMISSION_COLOR:
        return "EmissionColor"sv;
    case aiTextureType_METALNESS:
        return "Metalness"sv;
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "Roughness"sv;
    case aiTextureType_AMBIENT_OCCLUSION:
        return "Ao"sv;
    case aiTextureType_UNKNOWN:
        return "UNKNOW"sv;
    case aiTextureType_SHEEN:
        return "Sheen"sv;
    case aiTextureType_CLEARCOAT:
        return "ClearnCoat"sv;
    case aiTextureType_TRANSMISSION:
        return "Transmission"sv;
    case aiTextureType_MAYA_BASE:
        return "MayaBase"sv;
    case aiTextureType_MAYA_SPECULAR:
        return "MayaSpecular"sv;
    case aiTextureType_MAYA_SPECULAR_COLOR:
        return "MayaSpecularColor"sv;
    case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
        return "MayaSpecularRougness"sv;
    case aiTextureType_ANISOTROPY:
        return "Anisotropy"sv;
    case aiTextureType_GLTF_METALLIC_ROUGHNESS:
        return "Mettalic_roughness"sv;
    case _aiTextureType_Force32Bit:
        return "Force32bit"sv;
    default:
        break;
    }
}


namespace PC_EDITOR_CORE
{

    bool AssetsImporter::ImportModel(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const std::filesystem::path& _path)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);
        {
            PERF_REGION_SCOPED_NAME_DYNAMIC(_path.generic_string().c_str());
            m_filePath = _path;
            m_ImportFormat = FindImportFormat(_path);

            if (m_ImportFormat == ImportFormat::None)
            {
                return false;
            }

            Assimp::Importer importer;
            const aiScene* scene = nullptr;
            {
                PERF_REGION_SCOPED_NAMED("Read Imported File");
                PERF_REGION_COLOR(PerfRegion::EditorResource);
                // Load the model with common processing flags
                scene = importer.ReadFile(
                    _path.generic_string().c_str(),
                    aiProcess_FlipUVs |
                    aiProcess_Triangulate |
                    aiProcess_GenNormals |
                    aiProcess_CalcTangentSpace |
                    aiProcess_GenBoundingBoxes //| 
                    //aiProcess_OptimizeMeshes | 
                    //aiProcess_OptimizeGraph
                );
            }


            if (!scene || !scene->HasMeshes())
            {
                PC_LOGERROR("Failed to load model: {} \n {} ", _path.generic_string(), importer.GetErrorString());
                return false;
            }
            m_ImportObjectName = _path.filename().generic_string();

            {
                std::vector<std::future<void>> futurs;

                if (!ImportTextures(_Rhi, ThreadPool, &futurs, scene))
                {
                    PC_LOGERROR("Failed To Import Textures")
                        return false;
                }

                if (!ImportMeshesFromScene(_Rhi, ThreadPool, scene))
                {
                    PC_LOGERROR("Failed To Import Mesh From Scene")
                    return false;
                }

                {
                    PERF_REGION_SCOPED_NAMED("Wait Texture Load Futur");
                    PERF_REGION_COLOR(PerfRegion::EditorResource);
                    for (auto& f : futurs)
                    {
                        f.wait();
                    }
                    futurs.clear();
                }

                ResolveMaterial(scene);

            }

            {
                PERF_REGION_SCOPED;
                PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "Fetch ResourceUpdateBranchs");

                std::scoped_lock _(_Rhi.GetRhiContext().lock);
                for (auto& it : m_ResourceUpdateBranchs)
                {
                    *_Rhi.GetRhiContext().ResourceUpdateBranch_AssumeLock() = std::move(it);
                }
            }

            m_Succes = true;
        }

        

        return m_Succes;
    }

    const std::string& AssetsImporter::GetName() const
    {
        return m_ImportObjectName;
    }

    AssetsImporter::ImportFormat AssetsImporter::FindImportFormat(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();

        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (ext == ".gltf" || ext == ".glb")
            return ImportFormat::Gltf;

        if (ext == ".fbx")
            return ImportFormat::Fbc;

        if (ext == ".obj")
            return ImportFormat::Obj;

        return ImportFormat::None;

    }

    size_t AssetsImporter::LODFromMeshName(const aiString& _AiS) const
    {
        std::string_view meshName(_AiS.C_Str());

        size_t LodIndex = 0;

        const size_t LodPos = meshName.find("LOD");
        if (LodPos != std::string_view::npos)
        {
            // Start right after "LOD"
            const char* begin = meshName.data() + LodPos + 3;
            size_t EndOfLOD = std::string_view(begin).find('_');

            if (EndOfLOD != std::string_view::npos)
            {

                auto [ptr, ec] = std::from_chars(begin, begin + EndOfLOD, LodIndex);

                if (ec != std::errc{})
                {
                    PC_LOGERROR("Failed to parse LOD index from mesh name: {}", _AiS.C_Str());
                    LodIndex = 0;
                    return LodIndex;
                }
            }
            
        }

        return LodIndex;
    }


    AssetsImporter::MeshLetBuildOut AssetsImporter::BuildMeshlet(const std::span<PC_CORE::StaticMeshVertex>& _Verticies, const std::span<const uint32_t>& _Indices)
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
        size_t meshletCount = meshopt_buildMeshlets(
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

        // Finally, resize (or trim) the storage to fit the actual number of meshlets built.
        //  meshopt_buildMeshletsBound return the worst scnerio size
        // Shrink
        auto& last = MeshletsOpt[meshletCount - 1];
        MeshletVertexTrianglesIndex.resize(last.VertexOffset + last.VertexCount);
        meshletTriangles.resize(last.TriangleOffset + ((last.TriangleCount * 3 + 3) & ~3));
        MeshletsOpt.resize(meshletCount);


        std::vector<uint32_t> MeshletTrianglesU32;
        
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


        AssetsImporter::MeshLetBuildOut Out;
        Out.MeshletsOpt = std::move(MeshletsOpt);
        Out.MeshletVertexTrianglesIndex = std::move(MeshletVertexTrianglesIndex);
        Out.MeshletTrianglesU32 = std::move(MeshletTrianglesU32);

        return Out;
    }

    void AssetsImporter::GatherUniqueMeshes([[maybe_unsed]] PC_CORE::StaticMeshData* _Data, const aiScene* scene , const aiNode* node)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if (!mesh || mesh->mName.Empty())
                continue;

            size_t lodIndex = LODFromMeshName(mesh->mName);
            std::string UniqueMeshSection(mesh->mName.C_Str());

            if (auto it = m_UniqueMeshSectionWithLod.try_emplace(UniqueMeshSection).second)
            {
                m_UniqueMeshSectionOrder.push_back(UniqueMeshSection);
            }
            auto& it = m_UniqueMeshSectionWithLod[UniqueMeshSection];

            size_t LodMax = std::max(it.size(), lodIndex + 1);

            if (LodMax != it.size())
            {
                it.resize(LodMax);
            }
            
            it[lodIndex] = mesh;

            m_NativeVertexCount += mesh->mNumVertices;
            for (size_t i = 0; i < mesh->mNumFaces; i++)
                m_NativeIndiciesCount += mesh->mFaces[i].mNumIndices;

        }

        for (size_t i = 0; i < node->mNumChildren; i++)
            GatherUniqueMeshes(_Data, scene, node->mChildren[i]);

    }


    void AssetsImporter::ProcessMeshes(PC_CORE::Thread::ThreadPool& ThreadPool, PC_CORE::StaticMeshData* _Data, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);
        using OutOPTMesh = std::pair<std::vector<uint32_t>, std::vector<PC_CORE::StaticMeshVertex>>;
        std::vector<std::vector<std::future<OutOPTMesh>>> Futures;

        // Un Opt Data Resize Data
        std::vector<PC_CORE::StaticMeshVertex> UnOptVertices;
        UnOptVertices.reserve(m_NativeVertexCount);
        std::vector<uint32_t> UnOptIndices;
        UnOptIndices.reserve(m_NativeIndiciesCount);

        _Data->MeshSections.resize(m_UniqueMeshSectionWithLod.size());

        Futures.resize(m_UniqueMeshSectionWithLod.size());
        for (size_t mS = 0; mS < m_UniqueMeshSectionWithLod.size(); mS++)
        {
            auto& MeshSectionAssimpWithLOD = m_UniqueMeshSectionWithLod[m_UniqueMeshSectionOrder[mS]];
            PC_CORE::MeshSection& MeshSection = _Data->MeshSections[mS];
            MeshSection.MaterialIndex = MeshSectionAssimpWithLOD[0]->mMaterialIndex;

            MeshSection.LODs.resize(MeshSectionAssimpWithLOD.size());

            // Init mesh Section
            MeshSection.VerticesGlobal = PC_CORE::OffsetAndCount(UnOptVertices.size(), 0);
            MeshSection.IndicesGlobal = PC_CORE::OffsetAndCount(UnOptIndices.size(), 0);

            Futures[mS].resize(MeshSectionAssimpWithLOD.size());
            for (size_t lod = 0; lod < MeshSectionAssimpWithLOD.size(); lod++)
            {
                const aiMesh& AiMesh = *MeshSectionAssimpWithLOD[lod];
                PC_CORE::MeshLOD& LOD = _Data->MeshSections[mS].LODs[lod];

                LOD.VertexSection = PC_CORE::OffsetAndCount(MeshSection.VerticesGlobal.Count, static_cast<size_t>(AiMesh.mNumVertices));
                FillVertices(UnOptVertices, AiMesh);

                size_t MeshIndexCount = 0;
                for (size_t i = 0; i < AiMesh.mNumFaces; i++)
                    MeshIndexCount += AiMesh.mFaces[i].mNumIndices;
                
                LOD.IndicesSection = PC_CORE::OffsetAndCount(MeshSection.IndicesGlobal.Count, MeshIndexCount);
                FillIndices(UnOptIndices, AiMesh);

                LOD.AABB = MotionCore::Aabb<double>(Tbx::Vector3d(AiMesh.mAABB.mMin.x, AiMesh.mAABB.mMin.y, AiMesh.mAABB.mMin.z), 
                    (Tbx::Vector3d(AiMesh.mAABB.mMax.x, AiMesh.mAABB.mMax.y, AiMesh.mAABB.mMax.z)));

                // Increase Global Count Mesh Section
                MeshSection.VerticesGlobal.Count += LOD.VertexSection.Count;
                MeshSection.IndicesGlobal.Count += LOD.IndicesSection.Count;
            }
        }


        // PreReserveData
        std::vector<PC_CORE::StaticMeshVertex> OptVertices;
        OptVertices.reserve(UnOptVertices.size());
        std::vector<uint32_t> OptIndices;
        OptIndices.reserve(UnOptIndices.size());

        for (size_t mS = 0; mS < m_UniqueMeshSectionWithLod.size(); mS++)
        {
            const PC_CORE::MeshSection& MeshSection = _Data->MeshSections[mS];

            for (size_t lod = 0; lod < MeshSection.LODs.size(); lod++)
            {
                const PC_CORE::MeshLOD& LOD = _Data->MeshSections[mS].LODs[lod];

                std::span<const uint32_t> SubMeshSpanIndicies(
                    UnOptIndices.data() + MeshSection.IndicesGlobal.Offset + LOD.IndicesSection.Offset,
                    + LOD.IndicesSection.Count
                );
                std::span<const PC_CORE::StaticMeshVertex> SubMeshSpanVertices(
                    UnOptVertices.data() + MeshSection.VerticesGlobal.Offset + LOD.VertexSection.Offset,
                    LOD.VertexSection.Count
                );

                Futures[mS][lod] = ThreadPool.Enqueue(
                    [this, spanIndicies = SubMeshSpanIndicies, spanVerticies = SubMeshSpanVertices]() -> OutOPTMesh
                    {
                        return OptimiseMesh(spanVerticies, spanIndicies);
                    });
            }
        }

        for (size_t mS = 0; mS < m_UniqueMeshSectionWithLod.size(); mS++)
        {
            PC_CORE::MeshSection& MeshSection = _Data->MeshSections[mS];

            MeshSection.VerticesGlobal = PC_CORE::OffsetAndCount(OptVertices.size(), 0);
            MeshSection.IndicesGlobal = PC_CORE::OffsetAndCount(OptIndices.size(), 0);

            for (size_t lod = 0; lod < MeshSection.LODs.size(); lod++)
            {
                Futures[mS][lod].wait();
                PC_CORE::MeshLOD& LOD = _Data->MeshSections[mS].LODs[lod];
                auto OutOptMesh = Futures[mS][lod].get();

                // Recompute mesh offset and count
                LOD.IndicesSection = PC_CORE::OffsetAndCount(MeshSection.IndicesGlobal.Offset, OutOptMesh.first.size());
                LOD.VertexSection = PC_CORE::OffsetAndCount(MeshSection.VerticesGlobal.Offset, OutOptMesh.second.size());

                OptIndices.insert(OptIndices.end(), OutOptMesh.first.begin(), OutOptMesh.first.end());
                OptVertices.insert(OptVertices.end(), OutOptMesh.second.begin(), OutOptMesh.second.end());

                MeshSection.IndicesGlobal.Count += OutOptMesh.first.size();
                MeshSection.VerticesGlobal.Count += OutOptMesh.second.size();

            }
        }


        _Data->RenderData.Vertices = std::move(OptVertices);
        _Data->RenderData.Indices = std::move(OptIndices);
    }

    void AssetsImporter::FillVertices(std::vector<PC_CORE::StaticMeshVertex>& _Verticies, const aiMesh& _Meshes)
    {
        for (size_t v = 0; v < _Meshes.mNumVertices; v++)
        {
            PC_CORE::StaticMeshVertex Vertex{};
            Vertex.Position = Tbx::Vector3f{ _Meshes.mVertices[v].x, _Meshes.mVertices[v].y, _Meshes.mVertices[v].z };

            if (_Meshes.HasNormals())
                Vertex.Normal = Tbx::Vector3f{ _Meshes.mNormals[v].x, _Meshes.mNormals[v].y, _Meshes.mNormals[v].z };

            if (_Meshes.HasTextureCoords(0))
                Vertex.Uv = Tbx::Vector2f{ _Meshes.mTextureCoords[0][v].x, _Meshes.mTextureCoords[0][v].y };

            if (_Meshes.HasTangentsAndBitangents())
                Vertex.Tangent = Tbx::Vector3f{ _Meshes.mTangents[v].x, _Meshes.mTangents[v].y, _Meshes.mTangents[v].z };

            _Verticies.emplace_back(std::move(Vertex));
        }
    }

    void AssetsImporter::FillIndices(std::vector<uint32_t>& _Indices, const aiMesh& _Meshes)
    {
        for (size_t i = 0; i < _Meshes.mNumFaces; i++)
        {
            for (size_t j = 0; j < _Meshes.mFaces[i].mNumIndices; j++)
                _Indices.emplace_back(_Meshes.mFaces[i].mIndices[j]);
        }
    }


    void AssetsImporter::LoadMesheletFromScene(PC_CORE::Thread::ThreadPool& ThreadPool, PC_CORE::StaticMeshData* _Data, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        if (!_Data)
            return;
        std::vector<std::vector<std::future<AssetsImporter::MeshLetBuildOut>>> Futures;

        for (size_t mS = 0; mS < m_UniqueMeshSectionWithLod.size(); mS++)
        {
            PC_CORE::MeshSection& MeshSection = _Data->MeshSections[mS];
            Futures.emplace_back();
            for (size_t lod = 0; lod < MeshSection.LODs.size(); lod++)
            {
                PC_CORE::MeshLOD& LOD = _Data->MeshSections[mS].LODs[lod];


                std::span<const uint32_t> subMeshIndicies(_Data->RenderData.Indices.data() + LOD.IndicesSection.Offset, LOD.IndicesSection.Count);
                std::span<PC_CORE::StaticMeshVertex> subMeshVerticies(_Data->RenderData.Vertices.data() + LOD.VertexSection.Offset, LOD.VertexSection.Count);

                Futures.back().emplace_back(ThreadPool.Enqueue([this, SubMeshIndicies = subMeshIndicies, SubMeshVerticies = subMeshVerticies]()
                    {
                        return BuildMeshlet(SubMeshVerticies, SubMeshIndicies);
                    }));

            }
        }

        {
            PERF_REGION_SCOPED;
            PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "Wait futures");
        
            for (size_t mS = 0; mS < m_UniqueMeshSectionWithLod.size(); mS++)
            {
                PC_CORE::MeshSection& MeshSection = _Data->MeshSections[mS];

                MeshSection.MeshletsGlobal = PC_CORE::OffsetAndCount(_Data->RenderData.Meshlets.size(), 0);
                MeshSection.MeshletsTriangleVertexIndexGlobal = PC_CORE::OffsetAndCount(_Data->RenderData.MeshletVertexTrianglesIndex.size(), 0);
                MeshSection.MeshletsTrianglesGlobal = PC_CORE::OffsetAndCount(_Data->RenderData.MeshletTriangles.size(), 0);

                for (size_t lod = 0; lod < MeshSection.LODs.size(); lod++)
                {
                    PC_CORE::MeshLOD& LOD = _Data->MeshSections[mS].LODs[lod];
                    auto& F = Futures[mS][lod];

                    F.wait();
                    AssetsImporter::MeshLetBuildOut Data(F.get());

                    LOD.MeshletsSection = PC_CORE::OffsetAndCount(MeshSection.MeshletsGlobal.Offset, Data.MeshletsOpt.size());
                    LOD.MeshletsTriangleVertexIndexSection = PC_CORE::OffsetAndCount(MeshSection.MeshletsTriangleVertexIndexGlobal.Offset, Data.MeshletVertexTrianglesIndex.size());
                    LOD.MeshletsTrianglesSection = PC_CORE::OffsetAndCount(MeshSection.MeshletsTrianglesGlobal.Offset, Data.MeshletTrianglesU32.size());


                    MeshSection.MeshletsGlobal.Count += LOD.MeshletsSection.Count;
                    MeshSection.MeshletsTriangleVertexIndexGlobal.Count += LOD.MeshletsTriangleVertexIndexSection.Count;
                    MeshSection.MeshletsTrianglesGlobal.Count += LOD.MeshletsTrianglesSection.Count;

                    _Data->RenderData.Meshlets.insert(_Data->RenderData.Meshlets.end(), Data.MeshletsOpt.begin(), Data.MeshletsOpt.end());
                    _Data->RenderData.MeshletVertexTrianglesIndex.insert(_Data->RenderData.MeshletVertexTrianglesIndex.end(), Data.MeshletVertexTrianglesIndex.begin(), Data.MeshletVertexTrianglesIndex.end());
                    _Data->RenderData.MeshletTriangles.insert(_Data->RenderData.MeshletTriangles.end(), Data.MeshletTrianglesU32.begin(), Data.MeshletTrianglesU32.end());
                }
            }
        }    
    }

    std::pair<std::vector<uint32_t>, std::vector<PC_CORE::StaticMeshVertex>> AssetsImporter::OptimiseMesh
    (   
        const std::span<const PC_CORE::StaticMeshVertex>& UnOptVertices,
        const std::span<const uint32_t>& UnOptIndices
    )
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);
        
        const size_t NumIndicies = UnOptIndices.size();
        const size_t NumVerticies = UnOptVertices.size();
        constexpr size_t SizeOfVertex = sizeof(std::remove_cv_t<std::remove_reference_t<decltype(UnOptVertices)>>::value_type);

        std::vector<uint32_t> Remap(NumVerticies);
        const size_t OptVerticesCount = meshopt_generateVertexRemap(
            Remap.data(),
            UnOptIndices.data(),
            NumIndicies,
            UnOptVertices.data(),
            NumVerticies,
            SizeOfVertex);

        std::vector<uint32_t> OptIndicies {};
        std::vector<PC_CORE::StaticMeshVertex> OptVerticies{};
        OptIndicies.resize(NumIndicies);
        OptVerticies.resize(OptVerticesCount);

        // remove duplicate Indicies
        meshopt_remapIndexBuffer(OptIndicies.data(), UnOptIndices.data(), NumIndicies, Remap.data());
        meshopt_remapVertexBuffer(OptVerticies.data(), UnOptVertices.data(), NumVerticies, SizeOfVertex, Remap.data());


        meshopt_optimizeVertexCache(OptIndicies.data(), OptIndicies.data(), NumIndicies, OptVerticesCount);

        meshopt_optimizeOverdraw(OptIndicies.data(), OptIndicies.data(), NumIndicies, &OptVerticies[0].Position.x, OptVerticesCount, SizeOfVertex, 1.05f);


        meshopt_optimizeVertexFetch(OptVerticies.data(), OptIndicies.data(), NumIndicies, OptVerticies.data(), OptVerticesCount, SizeOfVertex);
        /*
        const float Threshold = 0.5f;
        size_t TargetIndexCount = (size_t)(NumIndicies * Threshold);
        float TargetError = 0.2f;
        std::vector<uint32_t> SimplifiedIndicies(OptIndicies.size());
        size_t OptIndexCount = meshopt_simplify(SimplifiedIndicies.data(), OptIndicies.data(), NumIndicies,
            &OptVerticies[0].Position.x, OptVerticesCount, SizeOfVertex, TargetIndexCount, TargetError);

        SimplifiedIndicies.resize(OptIndexCount);
        */

        return std::pair<std::vector<uint32_t>, std::vector<PC_CORE::StaticMeshVertex>>(std::move(OptIndicies), std::move(OptVerticies));
    }


    bool AssetsImporter::ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        if (scene->mNumMeshes == 0)
            return false;
        PC_CORE::StaticMeshData StaticMeshData;

        GatherUniqueMeshes(&StaticMeshData, scene, scene->mRootNode);
        ProcessMeshes(ThreadPool, &StaticMeshData, scene);
        LoadMesheletFromScene(ThreadPool, &StaticMeshData, scene);
        {
            std::scoped_lock _(m_mutex);
            m_StaticMeshs = PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(m_ImportObjectName, StaticMeshData, &m_ResourceUpdateBranchs.emplace_back());
        }

        return true;
    }

    bool AssetsImporter::ImportTextures(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, std::vector<std::future<void>>* Futures, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        auto AiTextureToTexture2D = [&](
            aiString&& textureName,
            aiTextureType type)
            ->void
            {    
                PERF_REGION_SCOPED;
                PERF_REGION_COLOR(PerfRegion::EditorResource);

                const aiTexture* embeded = scene->GetEmbeddedTexture(textureName.C_Str());

                std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>> pair{};

                if (embeded) // HandleEmbeded Texture
                {
                    std::unique_ptr<PC_CORE::RhiTexture> texture(RhiTextureFromAiTexture(_Rhi, textureName.C_Str(), *embeded, type));
                    PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture));

                    if (!texture || !texture2D)
                        return;

                    pair.first = type;
                    pair.second = texture2D;
                    {
                        std::scoped_lock _(m_mutex);
                        m_TextureMaps[textureName.C_Str()] = std::move(pair);
                    }
                }
                else // FROM PATH
                {
                    const auto texturePath = m_filePath.parent_path() / std::filesystem::u8path(textureName.C_Str());
                    if (std::filesystem::exists(texturePath))
                    {
                        std::string pathString = texturePath.generic_string();
                        PC_CORE::Image image(pathString.c_str(), PC_CORE::RhiChannel::Rgba);
                        std::unique_ptr<PC_CORE::RhiTexture> texture(_Rhi.CreateTexture());

                        if (!texture || !image)
                            return;
                        
                        texture->SetName(textureName.C_Str());
                        BuildRhiTextureFromImage(_Rhi, *texture, &image, type, pathString.find(".png") != std::string::npos); // jpg dont use alpha 
                        PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture));

                        pair.first = type;
                        pair.second = texture2D;

                        {
                            std::scoped_lock _(m_mutex);
                            m_TextureMaps[textureName.C_Str()] = std::move(pair);
                        }
                    }
                }      
            };


        for (size_t i = 0; i < scene->mNumMaterials; i++)
        {
            if (scene->mMaterials[i] == nullptr)
                continue;

            for (size_t j = 0; j < static_cast<size_t>(AI_TEXTURE_TYPE_MAX); j++)
            {
                const aiTextureType type = static_cast<aiTextureType>(j);
                const size_t TextureCount = scene->mMaterials[i]->GetTextureCount(type);
                for (size_t k = 0; k < TextureCount; k++)
                {

                    aiString str;
                    if (scene->mMaterials[i]->GetTexture(type, k, &str) != aiReturn::aiReturn_SUCCESS)
                    {
                        continue;
                    }
                            
                    {
                        std::scoped_lock _(m_mutex);
                        if (m_TextureMaps.contains(str.C_Str()))
                        {
                            continue;
                        }
                        m_TextureMaps.emplace(str.C_Str(), std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>());
                    }        
                    Futures->emplace_back(ThreadPool.Enqueue(AiTextureToTexture2D, std::move(str), type));
                }
            }
              
        }

        return true;
    }

 
    void AssetsImporter::ResolveMaterial(const aiScene* scene)
    {
        std::vector<PC_CORE::ObjectPtr<PC_CORE::Rendering::Material>> Materials;
        Materials.resize(scene->mNumMaterials);
      
        for (size_t i = 0; i < Materials.size(); i++)
        {
            std::string materialName;

            aiString str = scene->mMaterials[i]->GetName();
            if (str.Empty())
            {
                materialName = m_StaticMeshs->Name + " Material " + std::to_string(i);
            }
            else
            {
                materialName = std::string(str.C_Str());
            }

            Materials[i] = PC_CORE::ResourceManager::Create<PC_CORE::Rendering::Material>(materialName);
        }

        for (size_t i = 0; i < scene->mNumMaterials; i++)
        {
            if (scene->mMaterials[i] == nullptr)
                continue;

            aiMaterial& Material = *scene->mMaterials[i];
            PC_CORE::Rendering::Material& CoreMaterial = *Materials[i];
            FillMaterialTexture(CoreMaterial, *scene->mMaterials[i]);

            {
                aiColor4D color;
                bool hasColor = false;

                // Prefer PBR base color
                if (Material.Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
                {
                    hasColor = true;
                }
                // Fallback to legacy diffuse
                else if (Material.Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                {
                    hasColor = true;
                }

                if (hasColor)
                {
                    CoreMaterial.SetAlbedoFactor(Tbx::Vector4f{
                        color.r, color.g, color.b, color.a
                        });
                }
            }
            
            float metallic = 0.0f;
            if (Material.Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS)
            {
                CoreMaterial.SetMetallicFactor(metallic);
            }

            float roughness = 1.0f;
            if (Material.Get(AI_MATKEY_METALLIC_FACTOR, roughness) == AI_SUCCESS)
            {
                CoreMaterial.SetRoughnessFactor(roughness);
            }

            float anisotropy = 1.0f;
            if (Material.Get(AI_MATKEY_ANISOTROPY_FACTOR, anisotropy) == AI_SUCCESS)
            {
                CoreMaterial.SetRoughnessFactor(anisotropy);
            }

            aiColor3D emmisive;
            if (Material.Get(AI_MATKEY_COLOR_EMISSIVE, emmisive) == AI_SUCCESS)
            {
                CoreMaterial.SetEmmisiveFactor(Tbx::Vector3f(emmisive.r, emmisive.g, emmisive.b));
            }

            float opacity = 1.0f;
            if (AI_SUCCESS == Material.Get(AI_MATKEY_OPACITY, opacity)) {
                if (opacity < 1.0f) {
                      // Material is transparent
                    CoreMaterial.SetMaterialType(PC_CORE::Rendering::MaterialType::Transparent);
                    CoreMaterial.SetUseAlpha(true);
                }
            }
        }

        for (auto& m : Materials)
        {
            m->Build();
        }

        if (m_StaticMeshs)
        {
            m_StaticMeshs->SetBaseMaterials(Materials);
        }
    }

    void AssetsImporter::FillMaterialTexture(PC_CORE::Rendering::Material& CoreMaterial, const aiMaterial& Material)
    {

        for (size_t j = 0; j < static_cast<size_t>(AI_TEXTURE_TYPE_MAX); j++)
        {
            const aiTextureType type = static_cast<aiTextureType>(j);

            switch (type)
            {
            case aiTextureType_NONE:
                continue;
            case aiTextureType_BASE_COLOR:// PBR albedo
            case aiTextureType_DIFFUSE:
            case aiTextureType_OPACITY:
                break;
            case aiTextureType_METALNESS: // metallic
                break;
            case aiTextureType_DIFFUSE_ROUGHNESS: // Rouhness
                break;
            case aiTextureType_NORMAL_CAMERA:
            case aiTextureType_NORMALS: // Normal
                break;
            case aiTextureType_EMISSION_COLOR:
            case aiTextureType_EMISSIVE: // Emisive
                break;
            case aiTextureType_LIGHTMAP: // AO
            case aiTextureType_AMBIENT_OCCLUSION:
                break;
            case aiTextureType_GLTF_METALLIC_ROUGHNESS:
            case aiTextureType_SPECULAR:
            case aiTextureType_HEIGHT:
            case aiTextureType_SHININESS:
            case aiTextureType_DISPLACEMENT:
            case aiTextureType_AMBIENT:
            case aiTextureType_REFLECTION:
            case aiTextureType_UNKNOWN:
            default:
                PC_LOGERROR("Ignore texture when build material {} type was {}", CoreMaterial.Name, AssimpTextureTypeToString(type).data());
                continue;
                break;
            }

            aiString textureName;

            const size_t TextureCount = Material.GetTextureCount(type);
            for (size_t k = 0; k < TextureCount; k++)
            {
                if (Material.GetTexture(type, k, &textureName) == aiReturn::aiReturn_SUCCESS)
                {
                    auto& pair = m_TextureMaps[std::string(textureName.C_Str())].second;
                    auto Texture = pair.Lock();
                    if (!Texture)
                        continue;

                    if (type == aiTextureType_DIFFUSE)
                    {
                        CoreMaterial.SetAlbedoTexture(Texture);
                        if (Texture->Get()->UseAlpha())
                        {
                            CoreMaterial.SetMaterialType(PC_CORE::Rendering::MaterialType::Transparent);
                        }
                    }

                    if (type == aiTextureType_METALNESS ||
                        type == aiTextureType_DIFFUSE_ROUGHNESS ||
                        type == aiTextureType_AMBIENT_OCCLUSION)
                    {
                        CoreMaterial.SetMetallicRoughnessAOTexture(Texture);
                    }

                    if (type == aiTextureType_NORMAL_CAMERA || type == aiTextureType_NORMALS)
                        CoreMaterial.SetNormalTexture(Texture);

                    if (type == aiTextureType_EMISSIVE || type == aiTextureType_EMISSION_COLOR)
                        CoreMaterial.SetEmissiveTexture(Texture);
                }
            }

        }
    }

    PC_CORE::RhiTexture* AssetsImporter::RhiTextureFromAiTexture(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture, aiTextureType textureType)
    {
        PC_CORE::RhiTexture* RhiTexturePtr = _Rhi.CreateTexture();

        if (aiTexture.mHeight == 0) // Compressed
        {
            PC_CORE::Image image(reinterpret_cast<const uint8_t*>(aiTexture.pcData), static_cast<size_t>(aiTexture.mWidth), TextureName, PC_CORE::RhiChannel::Rgba);
            RhiTexturePtr->SetName(TextureName);

            BuildRhiTextureFromImage(_Rhi, *RhiTexturePtr, &image, textureType, false); // TODO ALPHA
            return RhiTexturePtr;
        }
        else
        {
            PC_LOGERROR("Dont support raw texture")
        }

        return nullptr;
    }

    void AssetsImporter::BuildRhiTextureFromImage(PC_CORE::Rhi& _Rhi, PC_CORE::RhiTexture& _Texture, PC_CORE::Image* _Image, aiTextureType textureType, bool _UseApha)
    {
        assert(!_Image->IsHdr());

        _Texture
            .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetTextureUsage(PC_CORE::RhiTexture::TextureUsageFlagBits::Sampled | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferDst
                | PC_CORE::RhiTexture::TextureUsageFlagBits::LoadAndStore | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferSrc)
            .SetTextureType(PC_CORE::RhiTexture::Type::Texture2D)
            .SetWidth(_Image->GetWidht())
            .SetHeight(_Image->GetHeight())
            .SetLevel(static_cast<uint32_t>(std::floor(std::log2(std::max(_Image->GetWidht(), _Image->GetHeight())))) + 1)
            .SetUseAlpha(_UseApha);

   
        switch (_Image->GetChannel())
        {
        case PC_CORE::RhiChannel::Rgb:
        case PC_CORE::RhiChannel::Rgba:

                _Texture.SetRhiFormat(_Image->IsHdr()
                    ? PC_CORE::RhiFormat::R16G16B16A16Sfloat
                    : PC_CORE::RhiFormat::R8G8B8A8Unorm);

            break;
        default:
            assert(false && "NotSupported");
            break;
        }

        _Texture.Build();


        {
            std::scoped_lock _(m_mutex);
            PC_CORE::RHI::ResourceUpdateBranch* updateBranch(&m_ResourceUpdateBranchs.emplace_back());
            updateBranch
                ->TextureUpload2D(_Texture,
                    _Image->Release(),
                    _Image->GetSizeInBytes(),
                    RhiResourceState::CopyDst)
                .GenerateMipmap(
                    _Texture,
                    PC_CORE::Filter::Linear,
                    RhiResourceState::FragmentShaderResource);
        }
        
        
    }
    
}