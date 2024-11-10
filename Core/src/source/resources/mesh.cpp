#include "resources/mesh.hpp"


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"


//#include <OBJ_Loader.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace PC_CORE;

Mesh::Mesh(const fs::path& _path) : Resource(_path)
{
    uint32_t formatIndex = -1;

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
        LoadObj(_path.generic_string(), verticies, indicies);
        break;
    default:;
    }

    format = MeshSourceFormat.at(formatIndex);

    nbrOfVertices = static_cast<uint32_t>(verticies.size());
    nbrOfIndices = static_cast<uint32_t>(indicies.size());

    vertexBuffer = VertexBuffer(verticies);
    indexBuffer = IndexBuffer(indicies);
}

Mesh::~Mesh()
{

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

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
    {
        throw std::runtime_error(err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                attrib.vertices[3 * index.normal_index + 0],
                attrib.vertices[3 * index.normal_index + 1],
                attrib.vertices[3 * index.normal_index + 2]
            };


            vertex.textureCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                _vertices.push_back(vertex);
            }

            _indices.push_back(uniqueVertices[vertex]);

          
        }
    }



    /*
    
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
    */

        
}
