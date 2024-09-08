#pragma once

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"

BEGIN_PCCORE

struct Vertex
{
    Tbx::Vector3f position;
    Tbx::Vector3f normal;
    Tbx::Vector2f textureCoord;

    PC_CORE_API bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && textureCoord == other.textureCoord;
    }

};

END_PCCORE



