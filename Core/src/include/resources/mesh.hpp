#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "rendering/vertex.hpp"
#include "rendering/vulkan/vulkan_index_buffer.hpp"
#include "rendering/vulkan/vulkan_vertex_buffer.hpp"

BEGIN_PCCORE
class Mesh : public IResource
{
public:

    enum class MeshFormat 
    {
        OBJ,
        GLTF,

        MESHFORMATMAX
    };

    static inline  std::array<std::string, 2> MeshSourceFormat
    {
        ".obj",
        ".gltf",
      };

    std::vector<Vertex> verticies;

    std::vector<uint32_t> indicies;

    VulkanVertexBuffer vulkanVertexBuffer;

    VulkanIndexBuffer vulkanIndexBuffer;

    MeshFormat meshFormat;
    
    ~Mesh() override;
    
    void Load(const fs::path& path) override;

private:
    void LoadObj(const std::string& path);

};

END_PCCORE