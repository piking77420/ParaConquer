#include "rendering/render_harware_interface/vertex.hpp"

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
