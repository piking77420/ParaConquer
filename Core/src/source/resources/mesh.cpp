#include "resources/mesh.hpp"
#include <OBJ_Loader.h>


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"

using namespace PC_CORE;

Mesh::~Mesh()
{
    vulkanVertexBuffer.Destroy();
    vulkanIndexBuffer.Destroy();
}

void Mesh::Load(const fs::path& path)
{
    std::string format = std::filesystem::path(path).extension().generic_string();
    uint32_t formatIndex = -1;
    std::string formatToString = path.generic_string();
    
    if (!IResource::IsFormatValid(MeshSourceFormat, format, &formatIndex))
    {
        return;
    }
    meshFormat = static_cast<MeshFormat>(formatIndex);

    switch (meshFormat)
    {
    case MeshFormat::OBJ:
        LoadObj(formatToString);
        break;
    default: ;
    }

    name = path.filename().generic_string();
    format = MeshSourceFormat.at(formatIndex);
    resourcePath = path;

    vulkanVertexBuffer.Init(verticies);
    vulkanIndexBuffer.Init(indicies);
}

void Mesh::LoadObj(const std::string& path)
{
    objl::Loader Loader;

    bool loadout = Loader.LoadFile(path.c_str());

    if (!loadout)
    {
        return;
    }
    // to do make it for mutilpel mesh
    for (int i = 0; i < 1; i++)
    {
        verticies.resize(Loader.LoadedMeshes[i].Vertices.size());

        // Copy one of the loaded meshes to be our current mesh
        const objl::Mesh& curMesh = Loader.LoadedMeshes[i];
        for (size_t i = 0; i < curMesh.Vertices.size(); i++)
        {
            
            verticies[i].position = Tbx::Vector3f(curMesh.Vertices[i].Position.X, curMesh.Vertices[i].Position.Y, curMesh.Vertices[i].Position.Z);
            verticies[i].normal = Tbx::Vector3f(curMesh.Vertices[i].Normal.X, curMesh.Vertices[i].Normal.Y, curMesh.Vertices[i].Normal.Z);
            verticies[i].textureCoord = Tbx::Vector2f(curMesh.Vertices[i].TextureCoordinate.X, curMesh.Vertices[i].TextureCoordinate.Y);
        }
        indicies = Loader.LoadedIndices;
    } 


        
}
