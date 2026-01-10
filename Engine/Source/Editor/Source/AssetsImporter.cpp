#include "AssetsImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <string_view>

#include "Rendering/Material.hpp"

#include "Resources/FileLoader.hpp"
#include "Resources/StaticMesh.hpp"
#include "Resources/ResourceManager.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Serialize/Serializer.h"

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

    bool AssetsImporter::ImportModel(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        m_filePath = _path;
        m_ImportFormat = FindImportFormat(_path);

        if (m_ImportFormat == ImportFormat::None)
        {
            return false;
        }

        Assimp::Importer importer;

        // Load the model with common processing flags
        const aiScene* scene = importer.ReadFile(
            _path.generic_string().c_str(),
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_ImproveCacheLocality
        );

        if (!scene || !scene->HasMeshes())
        {
            PC_LOGERROR("Failed to load model: {} \n {} ", _path.generic_string(), importer.GetErrorString());
            return false;
        }
        m_ImportObjectName = scene->mName.Empty() ? _path.filename().generic_string() : std::string(scene->mName.C_Str());

        {
            if (!ImportTextures(_Rhi, scene))
            {
                PC_LOGERROR("Failed To Import Textures")
                    return false;
            }

            if (!ImportMeshesFromScene(_Rhi, scene))
            {
                PC_LOGERROR("Failed To Import Mesh From Scene")
                    return false;
            }

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
        


        return true;
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
    bool AssetsImporter::ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        if (scene->mNumMeshes == 0)
            return false;

        PC_CORE::StaticMeshRenderData StaticMeshRenderData;
        // CountVertex And Index
        uint32_t nbrOfVerticies = 0;
        uint32_t nbrOfIndex = 0;
        StaticMeshRenderData.SubMeshes.reserve(scene->mNumMeshes);
        m_StaticMeshs.reserve(scene->mNumMeshes + 1);

      
        for (size_t i = 0; i < scene->mNumMeshes; i++)
        {
            uint32_t accFaceIndicies = 0;
            for (size_t f = 0; f < scene->mMeshes[i]->mNumFaces; f++)
            {
                accFaceIndicies += scene->mMeshes[i]->mFaces[f].mNumIndices;
            }

            PC_CORE::SubMesh subMesh =
            {
                .VertexOffSet = nbrOfVerticies,
                .VerticiesCount = scene->mMeshes[i]->mNumVertices,
                .IndexOffset = nbrOfIndex,
                .IndiciesCount = accFaceIndicies,
                .MaterialIndex = scene->mMeshes[i]->mMaterialIndex
            };

            nbrOfVerticies += subMesh.VerticiesCount;
            nbrOfIndex += subMesh.IndiciesCount;

            StaticMeshRenderData.SubMeshes.emplace_back(std::move(subMesh));
        }

        StaticMeshRenderData.Vertices.reserve(nbrOfVerticies);
        StaticMeshRenderData.Indices.reserve(nbrOfIndex);

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

                StaticMeshRenderData.Vertices.emplace_back(vertex);
            }

            for (size_t f = 0; f < mesh.mNumFaces; f++)
            {
                for (size_t i = 0; i < mesh.mFaces[f].mNumIndices; i++)
                {
                    StaticMeshRenderData.Indices.emplace_back(mesh.mFaces[f].mIndices[i]);
                }
            }
        }

        m_StaticMeshs.emplace_back(PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(m_ImportObjectName, StaticMeshRenderData, &m_ResourceUpdateBranchs.emplace_back()));
        for (size_t i = 0; i < scene->mNumMeshes; i++)
        {
            std::string meshName = scene->mMeshes[i]->mName.Empty() ? std::string(scene->mMeshes[i]->mName.C_Str()) : std::format("SubMesh {}", i);

            m_StaticMeshs.emplace_back(PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(m_ImportObjectName + " " + meshName, m_StaticMeshs[0], StaticMeshRenderData.SubMeshes[i]));
        }



        return true;
    }

    bool AssetsImporter::ImportTextures(PC_CORE::Rhi& _Rhi, const aiScene* scene)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::EditorResource);

        auto TextureFromType = [&](
            aiMaterial* mat,
            aiTextureType type)
            ->void
            {
                const size_t TextureCount = mat->GetTextureCount(type);
                for (size_t i = 0; i < TextureCount; i++)
                {
                    aiString str;
                    if (mat->GetTexture(type, i, &str) != aiReturn::aiReturn_SUCCESS)
                        continue;

                    if (m_TextureMaps.contains(str.C_Str()))
                        continue;

                    const aiTexture* aiTexture = scene->GetEmbeddedTexture(str.C_Str());
                    std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>> pair;

                    if (aiTexture) // HandleEmbeded Texture
                    {
                        std::unique_ptr<PC_CORE::RhiTexture> texture(RhiTextureFromAiTexture(_Rhi, str.C_Str(), *aiTexture));
                        PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture));


                        pair.first = type;
                        pair.second = texture2D;
                        m_TextureMaps.emplace(str.C_Str(), std::move(pair));
                    }
                    else // FROM PATH
                    {
                        const auto texturePath = m_filePath.parent_path() / std::filesystem::u8path(str.C_Str());
                        if (std::filesystem::exists(texturePath))
                        {
                            PC_CORE::Image image(texturePath.generic_string().c_str(), PC_CORE::RhiChannel::Rgba);
                            std::unique_ptr<PC_CORE::RhiTexture> texture(_Rhi.CreateTexture());
                            if (texture && image)
                            {
                                texture->SetName(str.C_Str());
                                BuildRhiTextureFromImage(_Rhi, *texture, &image);
                                PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture2D = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(std::move(texture));

                                pair.first = type;
                                pair.second = texture2D;
                                m_TextureMaps.emplace(str.C_Str(), std::move(pair));
                            }

                        }
                    }
                }

            };


        for (size_t i = 0; i < scene->mNumMaterials; i++)
        {
            if (scene->mMaterials[i] != nullptr)
                for (size_t j = 0; j < static_cast<size_t>(AI_TEXTURE_TYPE_MAX); j++)
                {
                    const aiTextureType type = static_cast<aiTextureType>(j);

                   TextureFromType(scene->mMaterials[i], type);
                }
        }


        return true;
    }

    bool AssetsImporter::ImportMaterials(PC_CORE::Rhi& _Rhi, const aiScene* scene)
    {




        return false;
    }

    PC_CORE::RhiTexture* AssetsImporter::RhiTextureFromAiTexture(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture)
    {
        PC_CORE::RhiTexture* RhiTexturePtr = _Rhi.CreateTexture();

        if (aiTexture.mHeight == 0) // Compressed
        {
            PC_CORE::Image image(reinterpret_cast<const uint8_t*>(aiTexture.pcData), static_cast<size_t>(aiTexture.mWidth), TextureName, PC_CORE::RhiChannel::Rgba);
            RhiTexturePtr->SetName(TextureName);

            BuildRhiTextureFromImage(_Rhi, *RhiTexturePtr, &image);
            return RhiTexturePtr;
        }
        else
        {
            PC_LOGERROR("Dont support raw texture")
        }

        return nullptr;
    }

    void AssetsImporter::BuildRhiTextureFromImage(PC_CORE::Rhi& _Rhi, PC_CORE::RhiTexture& _Texture, PC_CORE::Image* _Image)
    {
        assert(!_Image->IsHdr());

        _Texture
            .SetMemoryUsage(RhiMemoryUsage::Static)
            .SetTextureUsage(PC_CORE::RhiTexture::TextureUsageFlagBits::Sampled | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferDst
                | PC_CORE::RhiTexture::TextureUsageFlagBits::LoadAndStore | PC_CORE::RhiTexture::TextureUsageFlagBits::TransferSrc)
            .SetTextureType(PC_CORE::RhiTexture::Type::Texture2D)
            .SetWidth(_Image->GetWidht())
            .SetHeight(_Image->GetHeight())
            .SetLevel(static_cast<uint32_t>(std::floor(std::log2(std::max(_Image->GetWidht(), _Image->GetHeight())))) + 1);

        switch (_Image->GetChannel())
        {
        case PC_CORE::RhiChannel::Rgb:
        case PC_CORE::RhiChannel::Rgba:
            _Texture.SetRhiFormat(_Image->IsHdr() ? PC_CORE::RhiFormat::R16G16B16A16Sfloat : PC_CORE::RhiFormat::R8G8B8A8Unorm);
            break;
        default:
            assert(false && "NotSupported");
            break;
        }

        _Texture.Build();



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