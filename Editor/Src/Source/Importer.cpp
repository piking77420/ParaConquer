#include "Importer.hpp"

#include <filesystem>
#include <string_view>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Resources/FileLoader.hpp"

#include "Serialize/Serializer.h"

#include "Resources/StaticMesh.hpp"
#include "Resources/Texture2D.hpp"
#include "Resources/ResourceManager.hpp"


bool PC_EDITOR_CORE::Importer::Import(const std::filesystem::path& _path,  PC_CORE::Serializer* _serializer, PC_CORE::TypeId* _outId, PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    PERF_REGION_SCOPED
    
    std::string vformat = _path.filename().extension().generic_string();

    if (vformat == ".fbx" || vformat == ".gltf" || vformat == ".obj")
    {
       return ImportMesh(_path, _serializer, _outId, _outResource);
    }
    else if (vformat == ".png" || vformat == ".dds" || vformat == ".jpg")
    {
        int width;
        int height;

        PC_CORE::Channel channel;
        uint8_t* pixels = PC_CORE::FileLoader::LoadImage(_path.generic_string().c_str(), &width, &height, &channel, PC_CORE::Channel::RGBA);
        if (!pixels)
        {
            PC_LOGERROR("failed to load texture image!");
            return false;
        }

        PC_CORE::RHIFormat format = PC_CORE::RHIFormat::UNDEFINED;

        switch (channel)
        {
        case PC_CORE::Channel::RGB:
        case PC_CORE::Channel::RGBA:
            format = PC_CORE::RHIFormat::R8G8B8A8_UNORM;
            break;
        default:
            assert("false");
            break;
        }
        const size_t totalSize = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channel);

        const PC_CORE::CreateImageInfo createTextureInfo =
        {
            .width = width,
            .height = height,
            .depth = 1,
            .layerCount = 1,
            .mipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
            .textureType = PC_CORE::TextureType::Texture2D,
            .format = format,
            .channel = channel,
            .textureUsage = PC_CORE::TextureUsage::Sampled,
            .memoryVisibility = PC_CORE::MemoryLocalisation::GPU_Only,
            .samples = 1,
            .GenerateMipMap = true,
            .AllowCpuAcces = false,
            .datas = {reinterpret_cast<void*>(pixels)}
        };

        *_outResource = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(createTextureInfo);
        *_outId = PC_CORE::Reflector::GetTypeKey<PC_CORE::Texture2D>();

        PC_CORE::FileLoader::FreeData(pixels);
        return true;
    }
    else
    {
        PC_LOGERROR("can't import this file format {}", vformat);
    }


	return false;
}

bool PC_EDITOR_CORE::Importer::ImportMesh(const std::filesystem::path& _path,  PC_CORE::Serializer* _serializer,
    PC_CORE::TypeId* _outId, PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    PERF_REGION_SCOPED
    // to do skeleton mehs
    return ImportStaticMesh(_path, _serializer, _outId, _outResource);
}

bool PC_EDITOR_CORE::Importer::ImportStaticMesh(const std::filesystem::path& _path,
    PC_CORE::Serializer* _serializer, PC_CORE::TypeId* _outId,
    PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    PERF_REGION_SCOPED
      Assimp::Importer importer;

        std::string name = _path.filename().stem().generic_string();
        name += "StaticMesh";
        if (PC_CORE::ResourceManager::Exist(name))
        {
            return false;
        }

        // Load the model with common processing flags
        const aiScene* scene = importer.ReadFile(
            _path.generic_string(),
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_ImproveCacheLocality
        );

        if (!scene || !scene->HasMeshes())
        {
            PC_LOGERROR("Failed to load model: {} \n {} ", _path.generic_string(), importer.GetErrorString());
        }

        aiMesh* mesh = scene->mMeshes[0];

        std::vector<PC_CORE::StaticMeshVertex> vertices;
        std::vector<uint32_t> indices;

        vertices.reserve(mesh->mNumVertices);
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            PC_CORE::StaticMeshVertex v{};
            v.position = Tbx::Vector3f{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            if (mesh->HasNormals())
                v.normal = Tbx::Vector3f{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            if (mesh->HasTextureCoords(0))
                v.uv = Tbx::Vector2f{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

            if (mesh->HasTangentsAndBitangents())
                v.tangent = Tbx::Vector3f{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };

            vertices.push_back(v);
        }

        // Extract indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        const PC_CORE::StaticMeshCreateInfo staticMeshCreateInfo =
        {
            .name = std::move(name),
            .staticMeshRenderData = {vertices, indices},
            .hallowCpuAcces = false
        };
    
    *_outId = PC_CORE::Reflector::GetTypeKey<PC_CORE::StaticMesh>();
    *_outResource = PC_CORE::ObjectPtr(new PC_CORE::StaticMesh(staticMeshCreateInfo));

    _serializer->Serialize(staticMeshCreateInfo.staticMeshRenderData);
}

