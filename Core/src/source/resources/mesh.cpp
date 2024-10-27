#include "resources/mesh.hpp"


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"


#include <OBJ_Loader.h>

using namespace PC_CORE;

Mesh::~Mesh()
{

}

void Mesh::SetPath(const fs::path& _path)
{
    path = _path;
    
    std::string format = std::filesystem::path(path).extension().generic_string();
    uint32_t formatIndex = -1;
    std::string formatToString = path.generic_string();

    std::vector<Vertex> verticies;
    std::vector<uint32_t> indicies;
    
    if (!Resource::IsFormatValid(MeshSourceFormat, format, &formatIndex))
    {
        return;
    }
    meshFormat = static_cast<MeshFormat>(formatIndex);

    switch (meshFormat)
    {
    case MeshFormat::OBJ:
        LoadObj(formatToString,verticies, indicies);
        break;
    default: ;
    }

    name = path.filename().generic_string();
    format = MeshSourceFormat.at(formatIndex);

    nbrOfVertices = static_cast<uint32_t>(verticies.size());
    nbrOfIndices = static_cast<uint32_t>(indicies.size());
    
    vertexBuffer = VertexBuffer(verticies);
    indexBuffer = IndexBuffer(indicies);
}

uint32_t Mesh::GetNbrOfVerticies() const
{
    return nbrOfVertices;
}

uint32_t Mesh::GetNbrOfIndicies() const
{
    return nbrOfIndices;
}

void Mesh::LoadObj(const std::string& path, std::vector<Vertex>& _vertices, std::vector<uint32_t>& _indices)
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
        _vertices.resize(Loader.LoadedMeshes[i].Vertices.size());

        // Copy one of the loaded meshes to be our current mesh
        const objl::Mesh& curMesh = Loader.LoadedMeshes[i];
        for (size_t i = 0; i < curMesh.Vertices.size(); i++)
        {
            
            _vertices[i].position = Tbx::Vector3f(curMesh.Vertices[i].Position.X, curMesh.Vertices[i].Position.Y, curMesh.Vertices[i].Position.Z);
            _vertices[i].normal = Tbx::Vector3f(curMesh.Vertices[i].Normal.X, curMesh.Vertices[i].Normal.Y, curMesh.Vertices[i].Normal.Z);
            _vertices[i].textureCoord = Tbx::Vector2f(curMesh.Vertices[i].TextureCoordinate.X, curMesh.Vertices[i].TextureCoordinate.Y);
        }
        _indices = Loader.LoadedIndices;
    } 


        
}
