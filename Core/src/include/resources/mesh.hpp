#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/rhi_index_buffer.hpp"
#include "low_renderer/vertex.hpp"
#include "low_renderer/rhi_vertex_buffer.hpp"
#include "primitive/aabb.hpp"
#include "reflection/reflector.hpp"
#include "rendering/buffer/index_buffer.hpp"
#include "rendering/buffer/vertex_buffer.hpp"

BEGIN_PCCORE
 class Mesh : public Resource
{
public:

  
    VertexBuffer vertexBuffer;

    IndexBuffer indexBuffer;

    ModelFormat meshFormat;

    PC_CORE_API void LoadFromFile(const std::string& _path) override;

    PC_CORE_API IMP_DYNAMIC_REFLECT()

    PC_CORE_API void Build() override;

    PC_CORE_API Mesh(const std::string& _name);

    PC_CORE_API Mesh(const std::string& _name, const std::string& _path);

    PC_CORE_API Mesh();
    
    PC_CORE_API ~Mesh() override;

private:
    MotionCore::Aabb<double> m_Aabb;
        
    void LoadObj(const std::string& path, std::vector<Vertex>& _vertices, std::vector<uint32_t>& _indices);
};
REFLECT(Mesh, Resource)

END_PCCORE