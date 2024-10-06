#pragma once

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"
#include "shader_typedef.h"

BEGIN_PCCORE
    struct Vertex
{
    Tbx::Vector3f position;
    Tbx::Vector3f normal;
    Tbx::Vector2f textureCoord;

    PC_CORE_API bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && textureCoord == other.textureCoord;
    }
    
    static VertexInputBindingDescrition GetBindingDescrition(uint32_t _binding);

    static std::vector<VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding);
    
};

END_PCCORE



