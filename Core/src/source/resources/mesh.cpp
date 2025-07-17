#include "resources/mesh.hpp"


#include <unordered_map>

#include "log.hpp"
#include "physics/physics_wrapper.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace PC_CORE;



void Mesh::Build()
{
    
}

Mesh::Mesh(const std::string& _name) : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
}

Mesh::Mesh(const std::string& _name, const std::string& _path) : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    Mesh::LoadFromFile(_path);
}

Mesh::Mesh()
{
    DYNAMIC_REFLECT_INIT
}

Mesh::~Mesh()
{
}

void Mesh::LoadFromFile(const std::string& _path)
{
    Resource::LoadFromFile(_path);
    uint32_t formatIndex = -1;

    std::vector<Vertex> verticies;
    std::vector<uint32_t> indicies;

    if (!IsFormatValid(ModelSourceFormat, extension, &formatIndex))
    {
        return;
    }
    meshFormat = static_cast<ModelFormat>(formatIndex);

    LoadObj(_path, verticies, indicies);

    switch (meshFormat)
    {
    case ModelFormat::Obj:
        break;
    default:;
    }

    extension = ModelSourceFormat.at(formatIndex);
    vertexBuffer = VertexBuffer(verticies.data(), verticies.size(), PC_CORE::MemoryLocalisation::GPU_Only, MemoryUsage::Static);
    indexBuffer = IndexBuffer(indicies.data(), indicies.size(), MemoryUsage::Static);
    
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
