#include "rendering/render_harware_interface/vertex.hpp"

PC_CORE::VertexInputBindingDescrition PC_CORE::Vertex::GetBindingDescrition(uint32_t _binding)
{
    VertexInputBindingDescrition vertexInputBindingDescrition =
        {
            .binding = _binding,
            .stride = sizeof(Vertex),
            .vertexBindingDescriptions = {}
        };

    vertexInputBindingDescrition.vertexBindingDescriptions.resize(3);
    vertexInputBindingDescrition.vertexBindingDescriptions[0] = 
    {
    .binding = _binding,
    .location = 0,
    .format = RHIFormat::R32G32B32_SFLOAT,
    .offset = offsetof(Vertex, position)
    };

    vertexInputBindingDescrition.vertexBindingDescriptions[1] = 
    {
        .binding = _binding,
        .location = 1,
        .format = RHIFormat::R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, normal)
    };

    vertexInputBindingDescrition.vertexBindingDescriptions[2] = 
    {
        .binding = _binding,
        .location = 2,
        .format = RHIFormat::R32G32_SFLOAT,
        .offset = offsetof(Vertex, textureCoord)
    };

    return vertexInputBindingDescrition;
}
