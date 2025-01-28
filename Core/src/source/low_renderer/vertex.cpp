#include "low_renderer/vertex.hpp"

PC_CORE::Vertex::Vertex(Tbx::Vector3f _position, Tbx::Vector3f _normal, Tbx::Vector2f _textureCoord) : position(_position), normal(_normal), textureCoord(_textureCoord)
{
    
}

PC_CORE::VertexInputBindingDescrition PC_CORE::Vertex::GetBindingDescrition(uint32_t _binding)
{
    return
    {
        .binding = _binding,
        .stride = sizeof(Vertex),
        .vertexInputRate = VertexInputRate::VERTEX
    };
}

std::vector<PC_CORE::VertexAttributeDescription> PC_CORE::Vertex::GetAttributeDescriptions(uint32_t _binding)
{
    return
    {
        {
            .binding = _binding,
            .location = 0,
            .format = RHIFormat::R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, position)
        },
        {
            .binding = _binding,
            .location = 1,
            .format = RHIFormat::R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, normal)
        },
        {
            .binding = _binding,
            .location = 2,
            .format = RHIFormat::R32G32_SFLOAT,
            .offset = offsetof(Vertex, textureCoord)
        }


    };
}