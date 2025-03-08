#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/index_buffer.hpp"
#include "low_renderer/vertex.hpp"
#include "low_renderer/vertex_buffer.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
    class Mesh : public ResourceInterface<Mesh>
{
public:

    enum class MeshFormat : uint8_t
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


    VertexBuffer vertexBuffer;

    IndexBuffer indexBuffer;

    MeshFormat meshFormat;

    PC_CORE_API void Build() override;

    PC_CORE_API Mesh() = default;
    
    PC_CORE_API Mesh(const fs::path& _path);

    PC_CORE_API ~Mesh() override;

private:
    void LoadFromFile(const fs::path& _path);
    
    void LoadObj(const std::string& path, std::vector<Vertex>& _vertices, std::vector<uint32_t>& _indices);
};
    REFLECT(Mesh, Resource)

END_PCCORE