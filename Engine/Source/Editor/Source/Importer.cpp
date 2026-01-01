#include "Importer.hpp"

#include <filesystem>
#include <string_view>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Resources/FileLoader.hpp"
#include "Resources/StaticMesh.hpp"
#include "Resources/Texture2D.hpp"
#include "Resources/ResourceManager.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Serialize/Serializer.h"


namespace PC_EDITOR_CORE
{


bool PC_EDITOR_CORE::Importer::Import(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                      PC_CORE::TypeId* _outId,
                                      PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    PERF_REGION_SCOPED
   using namespace PC_CORE;

    std::string vformat = _path.filename().extension().generic_string();

    if (vformat == ".fbx" || vformat == ".gltf" || vformat == ".obj")
    {
        return ImportMesh(_Rhi, _path, _serializer, _outId, _outResource);
    }
    if (vformat == ".png" || vformat == ".dds" || vformat == ".jpg")
    {
        /*
        Image image(_path.generic_string().c_str(), RhiChannel::Rgba); 

        const RhiTexture::RhiTextureDesciptor desc =
        {
        .Width = image.GetWidht(),
        .Height = image.GetHeight(),
        .Depth = 1,
        .Level = static_cast<uint32_t>(std::floor(std::log2(std::max(image.GetWidht(), image.GetHeight())))) + 1,
        .LayerCount = 1,
        .Samples = 1,
        .TextureType = RhiTexture::Type::Texture2D,
        .TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::TransferDst),
        .RhiFormat = RhiFormat::R8G8B8A8Unorm, // TODO IF SRGB ON IN METADATA LOAD IN SRGB
        .AllowCpuAcces = false
        };

        PC_CORE::ObjectPtr<PC_CORE::Texture2D> texture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(_Rhi, _path.filename().generic_string(), desc, RhiResource::MemoryUsage::Static);
        texture->Get()->Build();

        _Rhi.PushResourceUpdate([&, image = std::move(image), texture](CommandList* _List)
            {
                texture->Get()->UploadData2D(_List, image.GetData(), image.GetWidht(), image.GetHeight(), image.GetChannel());
                texture->Get()->GenerateMipMap(_List);
            });
     

        *_outResource = texture;

        *_outId = PC_CORE::Reflector::GetTypeKey<PC_CORE::Texture2D>();*/

        return true;
    }
    PC_LOGERROR("can't import this file format {}", vformat);


    return false;
}

bool PC_EDITOR_CORE::Importer::ImportTexture(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                             PC_CORE::TypeId* _outId,
                                             PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    return false;
}

bool PC_EDITOR_CORE::Importer::ImportMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                          PC_CORE::TypeId* _outId,
                                          PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    PERF_REGION_SCOPED
    // to do skeleton mehs
    return ImportStaticMesh(_Rhi, _path, _serializer, _outId, _outResource);
}

bool PC_EDITOR_CORE::Importer::ImportStaticMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path,
                                                PC_CORE::Serializer* _serializer, PC_CORE::TypeId* _outId,
                                                PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const
{
    /*
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
        v.Position = Tbx::Vector3f{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        if (mesh->HasNormals())
            v.Normal = Tbx::Vector3f{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if (mesh->HasTextureCoords(0))
            v.Uv = Tbx::Vector2f{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

        if (mesh->HasTangentsAndBitangents())
            v.Tangent = Tbx::Vector3f{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};

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
        .Name = std::move(name),
        .StaticMeshRenderData = {std::move(vertices), std::move(indices)},
        .HallowCpuAcces = false
    };

    *_outId = PC_CORE::Reflector::GetTypeKey<PC_CORE::StaticMesh>();
    *_outResource = PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(staticMeshCreateInfo);


    PC_CORE::CompactBuffer verticies;
    verticies.CompressData(staticMeshCreateInfo.StaticMeshRenderData.Vertices.data(),
                           sizeof(PC_CORE::StaticMeshVertex) * staticMeshCreateInfo.StaticMeshRenderData.Vertices.
                           size());
    PC_CORE::CompactBuffer indicies;
    indicies.CompressData(staticMeshCreateInfo.StaticMeshRenderData.Indices.data(),
                          sizeof(staticMeshCreateInfo.StaticMeshRenderData.Indices[0]) * staticMeshCreateInfo.
                          StaticMeshRenderData.Indices.size());


    _serializer->SerializeCompactBuffer("StaticMeshRenderData Vertex", verticies);
    _serializer->SerializeCompactBuffer("StaticMeshRenderData Indicies", indicies);
    */
    return true;
}

bool Importer::ImportMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::StaticMesh* _StaticMesh) const
{
    PERF_REGION_SCOPED
    Assimp::Importer importer;

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
        v.Position = Tbx::Vector3f{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->HasNormals())
            v.Normal = Tbx::Vector3f{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->HasTextureCoords(0))
            v.Uv = Tbx::Vector2f{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

        if (mesh->HasTangentsAndBitangents())
            v.Tangent = Tbx::Vector3f{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };

        vertices.push_back(v);
    }

    // Extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    PC_CORE::StaticMeshRenderData StaticMeshRenderDat(vertices, indices);
    
    *_StaticMesh = PC_CORE::StaticMesh(std::string(mesh->mName.C_Str()), StaticMeshRenderDat);


    return false;
}

bool Importer::ImportTexture(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::ObjectPtr<PC_CORE::Texture2D>* _OutTexture2D) const
{
    return false;
}

}