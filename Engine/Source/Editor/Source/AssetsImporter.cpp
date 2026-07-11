#include "AssetsImporter.hpp"

#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <filesystem>
#include <string_view>

#include "Rendering/Material.hpp"

#include "LowRenderer/Rhi.hpp"
#include <Io/FileLoader.hpp>
#include "Resources/ResourceManager.hpp"
#include "Resources/StaticMesh.hpp"
#include "Serialize/Serializer.h"
#include "Thread/ThreadPool.hpp"

#include "meshoptimizer.h"
namespace PC_EDITOR_CORE
{

    size_t LODFromMeshName(const char* Name)
    {
        if (!Name)
            return 0;

        std::string_view meshName(Name);
        size_t LodIndex = 0;

        const size_t LodPos = meshName.find("LOD");
        if (LodPos == std::string_view::npos)
            return 0;

        // Position right after "LOD"
        const size_t numberStart = LodPos + 3;
        if (numberStart >= meshName.size())
            return 0;

        // Find end of digits (stop at '_' or end of string)
        const size_t numberEnd = meshName.find('_', numberStart);

        const char* begin = meshName.data() + numberStart;
        const char* end = (numberEnd == std::string_view::npos)
            ? meshName.data() + meshName.size()
            : meshName.data() + numberEnd;

        auto [ptr, ec] = std::from_chars(begin, end, LodIndex);

        if (ec != std::errc{} || ptr == begin)
        {
            PC_LOGERROR("Failed to parse LOD index from mesh name: {}", Name);
            return 0;
        }

        return LodIndex;
    }

AssetsImporter::ImportFormat FindImportFormat(const std::filesystem::path& path)
{
    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (ext == ".gltf" || ext == ".glb")
        return AssetsImporter::ImportFormat::Gltf;

    if (ext == ".fbx")
        return AssetsImporter::ImportFormat::Fbc;

    if (ext == ".obj")
        return AssetsImporter::ImportFormat::Obj;

    return AssetsImporter::ImportFormat::None;

}


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

    return "?";
}


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

            FetchResourcesUpdates(_Rhi);

            m_Succes = true;
        }

        return m_Succes;
    }

    PC_CORE::ObjectPtr<PC_CORE::Texture2D> AssetsImporter::ImportTexture(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path)
    {
        PC_CORE::ObjectPtr<PC_CORE::Texture2D> Texure2D = TextureFromPath(_Rhi, _path);
        if (!Texure2D)
            return Texure2D;

        FetchResourcesUpdates(_Rhi);
        return Texure2D;
    }

    PC_CORE::ObjectPtr<PC_CORE::Texture2D> AssetsImporter::TextureFromPath(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path)
    {
        std::string pathString = _path.generic_string();
        PC_CORE::Image image(pathString.c_str(), PC_CORE::RhiChannel::Rgba);
        PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D;

        if (!image)
        {
        error:
            PC_LOGERROR("Failed to import Texture {} ", _path.generic_string());
            return texture2D;
        }

        std::unique_ptr<PC_CORE::RhiTexture> texture(_Rhi.CreateTexture());
        texture->SetName(_path.filename().generic_string());
        BuildRhiTextureFromImage(_Rhi, *texture, &image, pathString.find(".png") != std::string::npos); // jpg dont use alpha 

        texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture), _path);

        if (!texture2D)
        {
            goto error;
        }

        return texture2D;
    }

    void AssetsImporter::FetchResourcesUpdates(PC_CORE::Rhi& _Rhi)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR_NAME(PerfRegion::EditorResource, "Fetch ResourceUpdateBranchs");

        std::scoped_lock _(_Rhi.GetRhiContext().ResourceUpdateLock());
        for (auto& it : m_ResourceUpdateBranchs)
            *_Rhi.GetRhiContext().ResourceUpdateBranch() = std::move(it);
    }

    const std::string& AssetsImporter::GetName() const
    {
        return m_ImportObjectName;
    }

    void PopuplateLodMeshMap(std::unordered_map<uint32_t, uint32_t>& AssimpMeshIndexToCoreIndex, std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>>& Map, const aiScene* Scene, uint32_t* LodMaxIndex)
    {
        for (uint32_t i = 0; i < Scene->mNumMeshes; i++)
        {
            const aiMesh* Mesh = Scene->mMeshes[i];
            const auto& Name = Mesh->mName;
            const uint32_t LodIndex = static_cast<uint32_t>(LODFromMeshName(Name.C_Str()));
            PC_LOG("Mesh Name {}", Name.C_Str());

            *LodMaxIndex = std::max(*LodMaxIndex, LodIndex);

            AssimpMeshIndexToCoreIndex[i] = Map[LodIndex].size();
            Map[LodIndex].emplace_back(i, Mesh->mMaterialIndex);
        }
    }

    void AssetsImporter::ProcessLod(
        std::unordered_map<uint32_t, uint32_t>& AssimpMeshIndexToCoreIndex,
        std::vector<PC_CORE::MeshLOD>& meshLods,
        const std::vector<MeshBuilder::MeshDescriptor>& MeshDescriptor,
        const PC_CORE::StaticMeshRenderData& RenderData,
        const aiScene* Scene)
    {
        PERF_REGION_SCOPED;
  
        std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> MeshAndMaterialIndexPerLods;
        uint32_t LodMaxIndex = 0;
        PopuplateLodMeshMap(AssimpMeshIndexToCoreIndex, MeshAndMaterialIndexPerLods, Scene, &LodMaxIndex);
        meshLods.resize(LodMaxIndex + 1);

        std::vector<uint32_t> sortedLods;
        for (auto& kv : MeshAndMaterialIndexPerLods)
            sortedLods.push_back(kv.first);

        std::sort(sortedLods.begin(), sortedLods.end());

        size_t curreentLod = 0;
        for (const auto& lodIndex : sortedLods)
        {
            const auto& LODMeshes = MeshAndMaterialIndexPerLods[lodIndex];
            auto& CurrentLod = meshLods[lodIndex];
            CurrentLod.MeshesSections.reserve(LODMeshes.size());
            PC_CORE::MeshDataDescriptor& LodDescriptor = CurrentLod.Descriptor;
            LodDescriptor = {};

            if (curreentLod != 0ull)
            {
                PC_CORE::MeshDataDescriptor& PrevLodDescriptor = meshLods[curreentLod - 1].Descriptor;

                LodDescriptor.VertexOffset =
                    PrevLodDescriptor.VertexOffset + PrevLodDescriptor.VertexCount;

                LodDescriptor.IndicesOffset =
                    PrevLodDescriptor.IndicesOffset + PrevLodDescriptor.IndicesCount;

                LodDescriptor.MeshetOffset =
                    PrevLodDescriptor.MeshetOffset + PrevLodDescriptor.MeshetCount;

                LodDescriptor.MeshletVertexTrianglesIndexOffset =
                    PrevLodDescriptor.MeshletVertexTrianglesIndexOffset + PrevLodDescriptor.MeshletVertexTrianglesIndexCount;

                LodDescriptor.MeshletTrianglesOffset =
                    PrevLodDescriptor.MeshletTrianglesOffset + PrevLodDescriptor.MeshletTrianglesCount;
            }

            // for each mesh In lod
            for (auto& [MeshIndex, MaterialIndex] : LODMeshes)
            {
                const auto& BaseMeshDescritptor = RenderData.BaseMeshDescriptor[MeshIndex];
                auto& MeshSection = CurrentLod.MeshesSections.emplace_back();

                MeshSection = 
                {
                    .MeshDataDescriptor = 
                    {
                        // Vertex
                        .VertexOffset = LodDescriptor.VertexCount,
                        .VertexCount = BaseMeshDescritptor.VertexCount,
                        // Indicies
                        .IndicesOffset = LodDescriptor.IndicesCount,
                        .IndicesCount = BaseMeshDescritptor.IndicesCount,

                        // Meshlets
                        .MeshetOffset = LodDescriptor.MeshetCount,
                        .MeshetCount = BaseMeshDescritptor.MeshetCount,

                        // MeshletTrianglesIndexOffset
                        .MeshletVertexTrianglesIndexOffset = LodDescriptor.MeshletVertexTrianglesIndexCount,
                        .MeshletVertexTrianglesIndexCount = BaseMeshDescritptor.MeshletVertexTrianglesIndexCount,

                        // MeshletTriangles
                        .MeshletTrianglesOffset = LodDescriptor.MeshletTrianglesCount,
                        .MeshletTrianglesCount = BaseMeshDescritptor.MeshletTrianglesCount,
                    },
                    .MaterialIndex = MaterialIndex
                };

                // OFFSET
                LodDescriptor.VertexCount += MeshSection.MeshDataDescriptor.VertexCount;
                LodDescriptor.IndicesCount += MeshSection.MeshDataDescriptor.IndicesCount;
                LodDescriptor.MeshetCount += MeshSection.MeshDataDescriptor.MeshetCount;
                LodDescriptor.MeshletVertexTrianglesIndexCount += MeshSection.MeshDataDescriptor.MeshletVertexTrianglesIndexCount;
                LodDescriptor.MeshletTrianglesCount += MeshSection.MeshDataDescriptor.MeshletTrianglesCount;
            }

            curreentLod++;
        }
    }

    static void ExpandAABB(aiAABB& box, const aiVector3D& p) {
        box.mMin.x = std::min(box.mMin.x, p.x);
        box.mMin.y = std::min(box.mMin.y, p.y);
        box.mMin.z = std::min(box.mMin.z, p.z);

        box.mMax.x = std::max(box.mMax.x, p.x);
        box.mMax.y = std::max(box.mMax.y, p.y);
        box.mMax.z = std::max(box.mMax.z, p.z);
    }

    static aiAABB transformAABB(const aiAABB& localBox, const aiMatrix4x4& transform) {
        aiAABB worldBox;

        const float inf = std::numeric_limits<float>::infinity();
        worldBox.mMin = aiVector3D(inf, inf, inf);
        worldBox.mMax = aiVector3D(-inf, -inf, -inf);

        const aiVector3D corners[8] = {
            {localBox.mMin.x, localBox.mMin.y, localBox.mMin.z},
            {localBox.mMax.x, localBox.mMin.y, localBox.mMin.z},
            {localBox.mMin.x, localBox.mMax.y, localBox.mMin.z},
            {localBox.mMax.x, localBox.mMax.y, localBox.mMin.z},

            {localBox.mMin.x, localBox.mMin.y, localBox.mMax.z},
            {localBox.mMax.x, localBox.mMin.y, localBox.mMax.z},
            {localBox.mMin.x, localBox.mMax.y, localBox.mMax.z},
            {localBox.mMax.x, localBox.mMax.y, localBox.mMax.z},
        };

        for (const aiVector3D& c : corners) {
            aiVector3D p = transform * c;
            ExpandAABB(worldBox, p);
        }

        return worldBox;
    }

    void ProcessDrawCommand(std::vector<PC_CORE::MeshLOD>& MeshLods,
        const std::unordered_map<uint32_t, uint32_t>& AssimpMeshIndexToCoreIndex, 
        const aiScene* Scene, 
        const aiNode* Node, 
        aiAABB* SceneGlobalBound,
        const aiMatrix4x4& ParentTransform)
    {
        if (Node == nullptr)
            return;
        
        const aiMatrix4x4 NodeModelTransform = ParentTransform * Node->mTransformation;

        Tbx::Matrix4x4d CoreTransfrom;
        for (size_t i = 0; i < 16; i++)
            CoreTransfrom[i] = static_cast<double>(*(NodeModelTransform[0] + i));
        CoreTransfrom = CoreTransfrom.Transpose(); // row to coloms
        CoreTransfrom[15] = 1.0; // just in case

        if (Node->mNumMeshes > 0u)
        {            
            for (size_t i = 0; i < Node->mNumMeshes; i++)
            {
                const aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
                const uint32_t LodIndex = LODFromMeshName(Mesh->mName.C_Str());

                const aiAABB TransformedNodeAABB = transformAABB(Mesh->mAABB, NodeModelTransform);
                ExpandAABB(*SceneGlobalBound, TransformedNodeAABB.mMin);
                ExpandAABB(*SceneGlobalBound, TransformedNodeAABB.mMax);

                auto& AABB = Scene->mMeshes[Node->mMeshes[i]]->mAABB;
                MotionCore::Aabb<double> CoreAABB = MotionCore::Aabb<double>(Tbx::Vector3d(AABB.mMin.x, AABB.mMin.y, AABB.mMin.z),
                    Tbx::Vector3d(AABB.mMax.x, AABB.mMax.y, AABB.mMax.z));
                
                CoreAABB = CoreAABB.GetTransformed(CoreTransfrom);
                MeshLods[LodIndex].DrawCommands.emplace_back(
                    CoreAABB,
                    CoreTransfrom,
                    AssimpMeshIndexToCoreIndex.at(Node->mMeshes[i])
                );
            }
        }
        
        for (size_t i = 0; i < Node->mNumChildren; i++)
            ProcessDrawCommand(MeshLods, AssimpMeshIndexToCoreIndex, Scene, Node->mChildren[i], SceneGlobalBound, NodeModelTransform);
        
    }

    bool AssetsImporter::ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const aiScene* Scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        if (Scene->mNumMeshes == 0)
            return false;
        
        constexpr bool BuildMeshlet = true;
        MeshBuilder::MeshBuilderData Meshs = BuildMeshs(ThreadPool, Scene, true);
        MeshBuilder::MeshletOutPutData Meshelets = BuildMeshlets(ThreadPool, Meshs);

        PC_CORE::StaticMeshRenderData StaticMeshRenderData;
        StaticMeshRenderData.Vertices = std::move(Meshs.Verticies);
        StaticMeshRenderData.Indices = std::move(Meshs.Indicies);
        StaticMeshRenderData.Meshlets = std::move(Meshelets.Meshlets);
        StaticMeshRenderData.MeshletVertexTrianglesIndex = std::move(Meshelets.MeshletVertexTrianglesIndex);
        StaticMeshRenderData.MeshletTriangles = std::move(Meshelets.MeshletTrianglesU32);
        StaticMeshRenderData.MeshletBound = std::move(Meshelets.MeshletsBound);

        assert(Meshs.MeshDescriptor.size() == Meshelets.MeshletDescriptor.size()); // there is meshelet build
        StaticMeshRenderData.BaseMeshDescriptor.reserve(Meshs.MeshDescriptor.size());

        for (size_t i = 0; i < Meshs.MeshDescriptor.size(); i++)
        {
            const MeshDescriptor& MeshDescriptor = Meshs.MeshDescriptor[i];
            const MeshletDescriptor* MeshletDescriptor = BuildMeshlet ? &Meshelets.MeshletDescriptor[i] : nullptr;

            StaticMeshRenderData.BaseMeshDescriptor.emplace_back(PC_CORE::MeshDataDescriptor{
                    // Vertex
                    .VertexOffset = MeshDescriptor.VertexOffset,
                    .VertexCount = MeshDescriptor.VertexCount,
                     // Indicies
                    .IndicesOffset = MeshDescriptor.IndicesOffset,
                    .IndicesCount = MeshDescriptor.IndicesCount,

                    // Meshlets
                    .MeshetOffset = BuildMeshlet ? MeshletDescriptor->MeshletOffset : 0u,
                    .MeshetCount = BuildMeshlet ? MeshletDescriptor->MeshletCount : 0u,

                    // MeshletTrianglesIndexOffset
                    .MeshletVertexTrianglesIndexOffset = BuildMeshlet ? MeshletDescriptor->MeshletVertexTriangleIndexOffset : 0u,
                    .MeshletVertexTrianglesIndexCount = BuildMeshlet ? MeshletDescriptor->MeshletVertexTriangleIndexCount : 0u,

                    // MeshletTriangles
                    .MeshletTrianglesOffset = BuildMeshlet ? MeshletDescriptor->MeshletTrianglesOffset : 0u,
                    .MeshletTrianglesCount = BuildMeshlet ? MeshletDescriptor->MeshletTrianglesCount : 0u,
                });
        }

        // Now we proceed to each local lods
        PC_CORE::StaticMeshData StaticMeshData;
        std::unordered_map<uint32_t, uint32_t> AssimpMeshIndexToCore;
        ProcessLod(AssimpMeshIndexToCore, StaticMeshData.MeshLods, Meshs.MeshDescriptor, StaticMeshRenderData, Scene);

        const auto Transform = aiMatrix4x4();
        assert(Transform.IsIdentity());
        aiAABB AABBBase{};
        ProcessDrawCommand(StaticMeshData.MeshLods, AssimpMeshIndexToCore, Scene, Scene->mRootNode, &AABBBase, Transform);

        StaticMeshData.AABB.min = Tbx::Vector3d(static_cast<double>(AABBBase.mMin.x), static_cast<double>(AABBBase.mMin.y), static_cast<double>(AABBBase.mMin.z));
        StaticMeshData.AABB.max = Tbx::Vector3d(static_cast<double>(AABBBase.mMax.x), static_cast<double>(AABBBase.mMax.y), static_cast<double>(AABBBase.mMax.z));

        StaticMeshData.RenderData = std::move(StaticMeshRenderData);
        {
            std::scoped_lock _(m_mutex);
            m_StaticMeshs = PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(m_ImportObjectName, StaticMeshData, &m_ResourceUpdateBranchs.emplace_back());
        }
        return true;
    }

    bool AssetsImporter::ImportTextures(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, std::vector<std::future<void>>* Futures, const aiScene* _Scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        auto AiTextureToTexture2D = [this, Rhi = &_Rhi, Scene = _Scene] (
            aiString&& textureName,
            aiTextureType type) 
            mutable
            ->void
            {    
                PERF_REGION_SCOPED;
                PERF_REGION_COLOR(PerfRegion::EditorResource);

                const aiTexture* embeded = Scene->GetEmbeddedTexture(textureName.C_Str());

                std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>> pair{};
                pair.first = type;

                if (embeded) // HandleEmbeded Texture
                {
                    std::unique_ptr<PC_CORE::RhiTexture> texture(RhiTextureFromAiTexture(*Rhi, textureName.C_Str(), *embeded));
                    
                    PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture));

                    if (!texture2D)
                        return;

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
                        pair.second = TextureFromPath(*Rhi, texturePath);
                    
                        {
                            std::scoped_lock _(m_mutex);
                            m_TextureMaps[textureName.C_Str()] = std::move(pair);
                        }
                    }
                }      
            };


        for (size_t i = 0; i < _Scene->mNumMaterials; i++)
        {
            if (_Scene->mMaterials[i] == nullptr)
                continue;

            for (size_t j = 0; j < static_cast<size_t>(AI_TEXTURE_TYPE_MAX); j++)
            {
                const aiTextureType type = static_cast<aiTextureType>(j);
                const size_t TextureCount = _Scene->mMaterials[i]->GetTextureCount(type);
                for (size_t k = 0; k < TextureCount; k++)
                {
                    aiString str;
                    if (_Scene->mMaterials[i]->GetTexture(type, k, &str) != aiReturn::aiReturn_SUCCESS)
                    {
                        continue;
                    }

                    if (str.length == 0)
                        continue;
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
            std::string matName;

            aiString str = scene->mMaterials[i]->GetName();
            if (str.Empty())
            {
                if (auto StaticMesh = m_StaticMeshs.Lock())
                {
                    matName = StaticMesh->Name + " Material " + std::to_string(i);
                }
            }
            else
            {
                if (auto StaticMesh = m_StaticMeshs.Lock())
                {
                    if (std::strcmp(str.C_Str(), "DefaultMaterial") == 0)
                    {
                        matName = std::string(str.C_Str()) + "_" + StaticMesh->Name;
                    }
                    else
                    {
                        matName = std::string(str.C_Str());
                    }
                }   
            }
            
            if (PC_CORE::ResourceManager::Exist(matName))
            {
                Materials[i] = PC_CORE::ResourceManager::Get<PC_CORE::Rendering::Material>(matName);
            }
            else
            {
                Materials[i] = PC_CORE::ResourceManager::Create<PC_CORE::Rendering::Material>(matName);
            }
            
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

        if (auto StaticMesh = m_StaticMeshs.Lock())
        {
            StaticMesh->SetBaseMaterials(Materials);
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
                break;
            case aiTextureType_HEIGHT:
            case aiTextureType_SHININESS:
            case aiTextureType_DISPLACEMENT:
            case aiTextureType_AMBIENT:
            case aiTextureType_REFLECTION:
            case aiTextureType_UNKNOWN:
            default:
                PC_LOG_VERBOSE("Ignore texture when build material {} type was {}", CoreMaterial.Name, AssimpTextureTypeToString(type).data());
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
                        type == aiTextureType_GLTF_METALLIC_ROUGHNESS || 
                        type == aiTextureType_SPECULAR)
                    {
                        CoreMaterial.SetMetallicRoughnessAOTexture(Texture);
                    }

                    if (type == aiTextureType_AMBIENT_OCCLUSION || type == aiTextureType_LIGHTMAP)
                    {
                        CoreMaterial.SetAoTexture(Texture);
                    }

                    if (type == aiTextureType_NORMAL_CAMERA || type == aiTextureType_NORMALS)
                        CoreMaterial.SetNormalTexture(Texture);

                    if (type == aiTextureType_EMISSIVE || type == aiTextureType_EMISSION_COLOR)
                        CoreMaterial.SetEmissiveTexture(Texture);
                }
            }

        }
    }

    PC_CORE::RhiTexture* AssetsImporter::RhiTextureFromAiTexture(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture)
    {
        PC_CORE::RhiTexture* RhiTexturePtr = _Rhi.CreateTexture();

        if (aiTexture.mHeight == 0) // Compressed
        {
            std::string_view view(TextureName);
            PC_CORE::Image image(view, reinterpret_cast<const uint8_t*>(aiTexture.pcData), static_cast<size_t>(aiTexture.mWidth), TextureName, PC_CORE::RhiChannel::Rgba);
            RhiTexturePtr->SetName(TextureName);

            BuildRhiTextureFromImage(_Rhi, *RhiTexturePtr, &image, view.find(".png") != std::string::npos);
            return RhiTexturePtr;
        }
        else
        {
            assert(false);
            PC_LOGERROR("Dont support raw texture")
        }
        delete RhiTexturePtr;
        return nullptr;
    }

    void AssetsImporter::BuildRhiTextureFromImage(PC_CORE::Rhi& _Rhi, PC_CORE::RhiTexture& _Texture, PC_CORE::Image* _Image, bool _UseApha)
    {
        const auto& ImageLevel = _Image->GetMipDescriptor();
        assert(!_Image->GetMipDescriptor().empty());
        
        const uint32_t Level = (ImageLevel.size() == 1) ? _Rhi.ComputeTextureLevel(ImageLevel[0].width, ImageLevel[0].height): _Image->GetMipDescriptor().size();

        _Texture
            .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetTextureUsage(PC_CORE::RhiTexture::TextureUsageFlagBits::Sampled | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferDst
                | PC_CORE::RhiTexture::TextureUsageFlagBits::LoadAndStore | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferSrc)
            .SetTextureType(PC_CORE::RhiTexture::Type::Texture2D)
            .SetWidth(ImageLevel[0].width)
            .SetHeight(ImageLevel[0].height)
            .SetLevel(Level)
            .SetUseAlpha(_UseApha);

   
        std::optional<PC_CORE::RhiFormat> BuildInFormat = _Image->GetBuildInFormat();

        if (BuildInFormat)
        {
            _Texture.SetRhiFormat(*BuildInFormat);
        }
        else
        {
            switch (_Image->GetChannel())
            {
            case PC_CORE::RhiChannel::Rgb:
            case PC_CORE::RhiChannel::Rgba:
            {
                _Texture.SetRhiFormat(_Image->IsHdr()
                    ? PC_CORE::RhiFormat::R32G32B32A32Sfloat
                    : PC_CORE::RhiFormat::R8G8B8A8Unorm);
            }
            break;
            default:
                assert(false && "NotSupported");
                break;
            }
        }
        
        _Texture.Build();
        {
            const std::vector<PC_CORE::Image::MipsDescriptor>& ImageMipDescriptor = _Image->GetMipDescriptor();
            std::vector<PC_CORE::RhiTexture::LevelUploadOperation> LevelUploadOperations;
            LevelUploadOperations.reserve(ImageMipDescriptor.size());

            for (size_t i = 0; i < ImageMipDescriptor.size(); i++)
            {
                auto& LevelOp = LevelUploadOperations.emplace_back();
                LevelOp.Width = ImageMipDescriptor[i].width;
                LevelOp.Height = ImageMipDescriptor[i].height;
                LevelOp.Offset = ImageMipDescriptor[i].offset;
                LevelOp.Size = ImageMipDescriptor[i].size;
            }

            std::scoped_lock _(m_mutex);
            PC_CORE::RHI::ResourceUpdateBranch* updateBranch(&m_ResourceUpdateBranchs.emplace_back());
        
            if (_Texture.GetLevel() == 1)
            {
                updateBranch
                    ->TextureUpload2D(_Texture,
                        _Image->Release(),
                        LevelUploadOperations,
                        RhiResourceState::PixelShaderResource);
            }
            else
            {
                if (_Image->GetMipDescriptor().size() == 1)
                {
                    if (PC_CORE::IsBcFormat(_Texture.GetRhiFormat()))
                        __debugbreak();

                    updateBranch
                        ->TextureUpload2D(_Texture,
                            _Image->Release(),
                            LevelUploadOperations,
                            RhiResourceState::CopyDst).
                        GenerateMipmap(
                            _Texture,
                            PC_CORE::Filter::Linear,
                            RhiResourceState::PixelShaderResource);
                    
                }
                else
                {
                    updateBranch
                        ->TextureUpload2D(_Texture,
                            _Image->Release(),
                            LevelUploadOperations,
                            RhiResourceState::PixelShaderResource);
                }
                
            }
                
        }
        
        
    }
    
}