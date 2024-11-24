#pragma once

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE
    struct Vertex
{
    Tbx::Vector3f position;
    Tbx::Vector3f normal;
    Tbx::Vector2f textureCoord;

    Vertex() = default;

    ~Vertex() = default;

    PC_CORE_API bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && textureCoord == other.textureCoord;
    }
    
    PC_CORE_API static VertexInputBindingDescrition GetBindingDescrition(uint32_t _binding);

    PC_CORE_API static std::vector<VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding);
    
};

END_PCCORE

namespace std {
    template<> struct hash<Tbx::Vector3f> {
        size_t operator()(Tbx::Vector3f const& vec) const {
            return hash<float>()(vec.x) ^ (hash<float>()(vec.y) << 1) ^ (hash<float>()(vec.z) << 2);
        }
    };

    template<> struct hash<Tbx::Vector2f> {
        size_t operator()(Tbx::Vector2f const& vec) const {
            return hash<float>()(vec.x) ^ (hash<float>()(vec.y) << 1);
        }
    };

    template<> struct hash<PC_CORE::Vertex> {
        size_t operator()(PC_CORE::Vertex const& vertex) const {
            return ((hash<Tbx::Vector3f>()(vertex.position) ^
                (hash<Tbx::Vector3f>()(vertex.normal) << 1)) >> 1) ^
                (hash<Tbx::Vector2f>()(vertex.textureCoord) << 1);
        }
    };
}


