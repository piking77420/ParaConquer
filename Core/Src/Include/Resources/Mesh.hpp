#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "LowRenderer/RhiIndexBuffer.hpp"
#include "LowRenderer/Vertex.hpp"
#include "LowRenderer/RhiVertexBuffer.hpp"
#include "Primitive/Aabb.hpp"
#include "Reflection/Reflector.hpp"
#include "Rendering/Buffer/IndexBuffer.hpp"
#include "Rendering/Buffer/VertexBuffer.hpp"

BEGIN_PCCORE
 class Mesh : public Resource
{
public:
    VertexBuffer vertexBuffer;

    IndexBuffer indexBuffer;

    ModelFormat meshFormat;

    PC_CORE_API void LoadFromFile(const std::string& _path) override;

    PC_CORE_API IMP_DYNAMIC_REFLECT()

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