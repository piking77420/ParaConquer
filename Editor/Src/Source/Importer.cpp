#include "Importer.hpp"

#include <filesystem>
#include <string_view>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Resources/StaticMesh.hpp"
#include "Resources/ResourceManager.hpp"


bool PC_EDITOR_CORE::Importer::Import(const std::filesystem::path& _path, PC_CORE::TypeId* _outId, PC_CORE::ResourceRef<PC_CORE::Resource>* _outResource) const
{
    std::string vformat = _path.filename().extension().generic_string();

    if (vformat == ".fbx" || vformat == ".gltf" || vformat == ".obj")
    {
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
            .verticies = std::move(vertices),
            .indicies = std::move(indices)
        };

        *_outId = PC_CORE::Reflector::GetTypeKey<PC_CORE::StaticMesh>();
        *_outResource = PC_CORE::ResourceManager::Create<PC_CORE::StaticMesh>(staticMeshCreateInfo);
        return true;
    }
    else if (vformat == ".png" || vformat == ".dds")
    {

    }


	return false;
}

