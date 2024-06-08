#include "resources/mesh.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


#include <unordered_map>

#include "log.hpp"

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
    
    if (!IResource::IsFormatValid(MeshSourceFormat,format,&formatIndex))
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
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        throw std::runtime_error(warn);
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

            vertex.textureCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };
        
        
            vertex.normal = {
                attrib.vertices[3 * index.normal_index + 0],
                attrib.vertices[3 * index.normal_index + 1],
                attrib.vertices[3 * index.normal_index + 2]
            };
  

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(verticies.size());
                verticies.push_back(vertex);
            }

            indicies.push_back(uniqueVertices[vertex]);
        }
    }
}
