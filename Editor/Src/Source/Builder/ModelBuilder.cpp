
#include <vector>
#include <string>

#include "Builder/ModelBuilder.hpp"

using namespace PC_EDITOR_CORE;

PC_CORE::StaticMesh ModelBuilder::CreateStaticMesh(const std::filesystem::path& _pathToFile)
{
    /*
    Assimp::Importer importer;

    // Load the model with common processing flags
    const aiScene* scene = importer.ReadFile(
        _pathToFile.string(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || !scene->HasMeshes())
    {
        PC_LOGERROR("Failed to load model: {} \n {} ", _pathToFile.string(), importer.GetErrorString());
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
        .name = _pathToFile.filename().stem().generic_string(),
        .verticies = std::move(vertices),
        .indicies = std::move(indices)
    };

	return PC_CORE::StaticMesh(std::move(staticMeshCreateInfo));*/
    return {};
}
