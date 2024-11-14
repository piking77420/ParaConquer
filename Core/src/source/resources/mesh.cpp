#include "resources/mesh.hpp"


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"


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
}
