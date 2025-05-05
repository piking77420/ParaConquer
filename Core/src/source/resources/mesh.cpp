#include "resources/mesh.hpp"


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace PC_CORE;

void Mesh::Build()
{
    if (!pathToFile.empty())
        LoadFromFile((fs::path)(pathToFile));
}

Mesh::Mesh(const fs::path& _path) : ResourceInterface(_path)
{
    LoadFromFile(_path);
}

Mesh::~Mesh()
{
}


void Mesh::LoadFromFile(const fs::path& _path)
{
    pathToFile = _path.generic_string();
    extension = _path.extension().generic_string();

    uint32_t formatIndex = -1;

    std::vector<Vertex> verticies;
    std::vector<uint32_t> indicies;

    if (!IsFormatValid(MeshSourceFormat, extension, &formatIndex))
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

    extension = MeshSourceFormat.at(formatIndex);
    vertexBuffer = VertexBuffer(verticies.data(), verticies.size() * sizeof(Vertex));
    indexBuffer = IndexBuffer(indicies.data(), indicies.size() * sizeof(uint32_t));
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

            vertex.position = Tbx::Vector3f(
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            );

            vertex.normal = Tbx::Vector3f(
                attrib.vertices[3 * index.normal_index + 0],
                attrib.vertices[3 * index.normal_index + 1],
                attrib.vertices[3 * index.normal_index + 2]
            );


            vertex.textureCoord = Tbx::Vector2f(
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            );

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                _vertices.push_back(vertex);
            }

            _indices.push_back(uniqueVertices[vertex]);

          
        }
    }        
}
