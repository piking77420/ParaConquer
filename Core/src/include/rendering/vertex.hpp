#pragma once
#include <vulkan/vulkan_core.h>

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"

BEGIN_PCCORE

struct Vertex
{
    Vector3f position;
    Vector3f normal;
    Vector2f textureCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, textureCoord);
        
        return attributeDescriptions;
    }
};

inline bool operator==(const Vertex& _v1,const Vertex& _v2){
    return _v1.position == _v2.position && _v1.normal == _v2.normal && _v1.textureCoord == _v2.textureCoord;
}


END_PCCORE

namespace std {
    using namespace PC_CORE;
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex& vertex) const {
            // Combine the hashes of pos, normal, and texCoord using a hash function
            size_t hashValue = 0;
            hash_combine(hashValue, vertex.position.x);
            hash_combine(hashValue, vertex.position.y);
            hash_combine(hashValue, vertex.position.z);
            hash_combine(hashValue, vertex.normal.x);
            hash_combine(hashValue, vertex.normal.y);
            hash_combine(hashValue, vertex.normal.z);
            hash_combine(hashValue, vertex.textureCoord.x);
            hash_combine(hashValue, vertex.textureCoord.y);
            return hashValue;
        }

        // Hash combine function to combine multiple hash values
        template <typename T>
        void hash_combine(size_t& seed, const T& value) const {
            std::hash<T> hasher;
            seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}