#pragma once

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE


enum class VertexInputRate
{
    VERTEX = 0,
    INSTANCE = 1,

    COUNT
};

struct VertexInputBindingDescrition
{
    uint32_t binding = 0;
    uint32_t stride = 0;
    VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
};

struct VertexAttributeDescription
{
    uint32_t binding = 0;
    uint32_t location = 0;
    RHIFormat format = RHIFormat::UNDEFINED;
    uint32_t offset = 0;
};



    struct Vertex
{
    Tbx::Vector3f position;
    Tbx::Vector3f normal;
    Tbx::Vector2f textureCoord;

    Vertex(Tbx::Vector3f _position, Tbx::Vector3f _normal, Tbx::Vector2f _textureCoord);
    

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


