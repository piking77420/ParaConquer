#include "AssetsImporter.hpp"

#include <filesystem>
#include <string_view>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Resources/Texture2d.hpp"
#include "Rendering/Material.hpp"


#include "Resources/FileLoader.hpp"
#include "Resources/StaticMesh.hpp"
#include "Resources/Texture2D.hpp"
#include "Resources/ResourceManager.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Serialize/Serializer.h"


namespace PC_EDITOR_CORE
{



bool AssetsImporter::ImportModel(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::StaticMesh* _StaticMesh)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::EditorResource);

    m_ImportFormat = FindImportFormat(_path);

    if (m_ImportFormat == ImportFormat::None)
        return false;

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
    

    PC_CORE::StaticMeshRenderData StaticMeshRenderData;
    ImportMeshesFromScene(scene, StaticMeshRenderData);
   

    std::string Name;
    if (scene->mName.Empty())
    {
        Name = scene->mNumMeshes > 0 ? std::string(scene->mMeshes[0]->mName.C_Str()) : _path.filename().generic_string();
    }
    else
    {
        Name = std::string(scene->mName.C_Str());
    }
    
 
    *_StaticMesh = PC_CORE::StaticMesh(std::move(Name), std::move(StaticMeshRenderData));

    return false;
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
bool AssetsImporter::ImportMeshesFromScene(const aiScene* scene, PC_CORE::StaticMeshRenderData& _StaticMeshRenderData)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::EditorResource);

    // CountVertex And Index
    uint32_t nbrOfVerticies = 0;
    uint32_t nbrOfIndex = 0;
    _StaticMeshRenderData.SubMeshes.reserve(scene->mNumMeshes);
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

        _StaticMeshRenderData.SubMeshes.emplace_back(std::move(subMesh));
    }

    _StaticMeshRenderData.Vertices.reserve(nbrOfVerticies);
    _StaticMeshRenderData.Indices.reserve(nbrOfIndex);


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

            _StaticMeshRenderData.Vertices.emplace_back(vertex);
        }

        for (size_t f = 0; f < mesh.mNumFaces; f++)
        {
            for (size_t i = 0; i < mesh.mFaces[f].mNumIndices; i++)
            {
                _StaticMeshRenderData.Indices.emplace_back(mesh.mFaces[f].mIndices[i]);
            }
        }
    }


    return true;
}
/*
bool AssetsImporter::ImportMaterial(const aiScene* scene, std::vector < PC_CORE::Rendering::Material > * _Material)
{
    
    _Material->reserve(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial& material = *scene->mMaterials[i];

        //_Material->at(0).SetMaterialAttributeData<MaterialAttribute::Color>(1.f); / /TODO FIX
    }



    return true;
}

*/
}