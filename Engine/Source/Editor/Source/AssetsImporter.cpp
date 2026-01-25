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

#define kMaxTriangles 124
#define kMaxVertices  64

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
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_GenNormals |
                    aiProcess_CalcTangentSpace |
                    aiProcess_ImproveCacheLocality |
                    aiProcess_GenBoundingBoxes
                );
            }


            if (!scene || !scene->HasMeshes())
            {
                PC_LOGERROR("Failed to load model: {} \n {} ", _path.generic_string(), importer.GetErrorString());
                return false;
            }
            m_ImportObjectName = scene->mName.Empty() ? _path.filename().generic_string() : std::string(scene->mName.C_Str());

            {
                std::vector<std::future<void>> futurs;

                if (!ImportTextures(_Rhi, ThreadPool, &futurs, scene))
                {
                    PC_LOGERROR("Failed To Import Textures")
                        return false;
                }

                if (!ImportMeshesFromScene(_Rhi, scene))
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

    void AssetsImporter::LoadMesh(PC_CORE::StaticMeshRenderData* _StaticMeshRenderData, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        assert(_StaticMeshRenderData);
        if (!_StaticMeshRenderData)
            return;
             
        // CountVertex And Index
        uint32_t nbrOfVerticies = 0;
        uint32_t nbrOfIndex = 0;
        _StaticMeshRenderData->SubMeshes.reserve(scene->mNumMeshes);

        // No job 
        // Fully single thread
        for (size_t i = 0; i < scene->mNumMeshes; i++)
        {
            uint32_t accFaceIndicies = 0;
            for (size_t f = 0; f < scene->mMeshes[i]->mNumFaces; f++)
            {
                accFaceIndicies += scene->mMeshes[i]->mFaces[f].mNumIndices;
            }

            const Tbx::Vector3d min = static_cast<Tbx::Vector3d>(Tbx::Vector3f(scene->mMeshes[i]->mAABB.mMin.x, scene->mMeshes[i]->mAABB.mMin.y, scene->mMeshes[i]->mAABB.mMin.z));
            const Tbx::Vector3d max = static_cast<Tbx::Vector3d>(Tbx::Vector3f(scene->mMeshes[i]->mAABB.mMax.x, scene->mMeshes[i]->mAABB.mMax.y, scene->mMeshes[i]->mAABB.mMax.z));

            PC_CORE::SubMesh subMesh =
            {
                .VertexOffSet = nbrOfVerticies,
                .VerticiesCount = scene->mMeshes[i]->mNumVertices,
                .IndexOffset = nbrOfIndex,
                .IndiciesCount = accFaceIndicies,
                .MaterialIndex = scene->mMeshes[i]->mMaterialIndex,
                .AABB = MotionCore::Aabb<double>(min, max),
            };

            nbrOfVerticies += subMesh.VerticiesCount;
            nbrOfIndex += subMesh.IndiciesCount;

            _StaticMeshRenderData->SubMeshes.emplace_back(std::move(subMesh));
        }

        _StaticMeshRenderData->Vertices.reserve(nbrOfVerticies);
        _StaticMeshRenderData->Indices.reserve(nbrOfIndex);

        for (size_t m = 0; m < scene->mNumMeshes; m++)
        {
            const aiMesh& mesh = *scene->mMeshes[m];
            for (size_t v = 0; v < mesh.mNumVertices; v++)
            {
                PC_CORE::StaticMeshVertex vertex{};
                vertex.Position = Tbx::Vector3f{ mesh.mVertices[v].x, mesh.mVertices[v].y, mesh.mVertices[v].z };

                if (mesh.HasNormals())
                    vertex.Normal = Tbx::Vector3f{ mesh.mNormals[v].x, mesh.mNormals[v].y, mesh.mNormals[v].z };

                if (mesh.HasTextureCoords(0))
                    vertex.Uv = Tbx::Vector2f{ mesh.mTextureCoords[0][v].x, mesh.mTextureCoords[0][v].y };

                if (mesh.HasTangentsAndBitangents())
                    vertex.Tangent = Tbx::Vector3f{ mesh.mTangents[v].x, mesh.mTangents[v].y, mesh.mTangents[v].z };

                _StaticMeshRenderData->Vertices.emplace_back(vertex);
            }

            for (size_t f = 0; f < mesh.mNumFaces; f++)
            {
                for (size_t i = 0; i < mesh.mFaces[f].mNumIndices; i++)
                {
                    _StaticMeshRenderData->Indices.emplace_back(mesh.mFaces[f].mIndices[i]);
                }
            }
        }
    }

    void AssetsImporter::OptimiseMesh(PC_CORE::StaticMeshRenderData& _StaticMeshRenderData)
    {
        std::vector<std::future<void>> MeshOptTask;

        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        std::vector<uint32_t> remap(_StaticMeshRenderData.Vertices.size());

        size_t TotalVertices;

        {
            PERF_REGION_SCOPED_NAMED("VertexRemap");

            TotalVertices = meshopt_generateVertexRemap(
                remap.data(),
                _StaticMeshRenderData.Indices.data(),
                (uint32_t)_StaticMeshRenderData.Indices.size(),
                _StaticMeshRenderData.Vertices.data(),
                (uint32_t)_StaticMeshRenderData.Vertices.size(),
                sizeof(PC_CORE::StaticMeshVertex)
            );
        }

        std::vector<PC_CORE::StaticMeshVertex> newVertices(TotalVertices);
        std::vector<uint32_t> newIndices(_StaticMeshRenderData.Indices.size());

        {
            PERF_REGION_SCOPED_NAMED("remap IndexBuffer And Vertex Buffer");
            meshopt_remapIndexBuffer(
                newIndices.data(),
                _StaticMeshRenderData.Indices.data(),
                _StaticMeshRenderData.Indices.size(),
                remap.data()
            );

            meshopt_remapVertexBuffer(
                newVertices.data(),
                _StaticMeshRenderData.Vertices.data(),
                _StaticMeshRenderData.Vertices.size(),
                sizeof(PC_CORE::StaticMeshVertex),
                remap.data()
            );

        }
       
        /*for (const auto& f : MeshOptTask)
        {
            f.wait();
        }*/
    }


    bool AssetsImporter::ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        if (scene->mNumMeshes == 0)
            return false;


        PC_CORE::StaticMeshRenderData StaticMeshRenderData;
        LoadMesh(&StaticMeshRenderData, scene);
        OptimiseMesh(StaticMeshRenderData);
        {
            std::scoped_lock _(m_mutex);
            m_StaticMeshs = PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(m_ImportObjectName, StaticMeshRenderData, &m_ResourceUpdateBranchs.emplace_back());
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