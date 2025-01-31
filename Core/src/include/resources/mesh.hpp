#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE
class Mesh : public Resource
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


    //VertexBuffer vertexBuffer;

    //IndexBuffer indexBuffer;

    MeshFormat meshFormat;
    
    PC_CORE_API Mesh(const fs::path& _path);

    PC_CORE_API ~Mesh() override;
    
    PC_CORE_API uint32_t GetNbrOfVerticies() const;

    PC_CORE_API uint32_t GetNbrOfIndicies() const;


private:
    void LoadObj(const std::string& path, std::vector<Vertex>& _vertices, std::vector<uint32_t>& _indices);

    uint32_t nbrOfVertices = 0;

    uint32_t nbrOfIndices = 0;
};

END_PCCORE